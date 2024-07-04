const express = require('express');
const fs = require('fs');
const cors = require('cors');
const bodyParser = require('body-parser');
const { PrismaClient } = require("@prisma/client");
const ServerlessHttp = require('serverless-http');

const prisma = new PrismaClient();
const app = express();
app.use(cors());
app.use(express.json())
const handler = ServerlessHttp(app)
let storedPostDataValue = null;
let storedSuhu = null;
let kipasData = {
    kelajuan: 150, // Set initial value to 150
};
let storedUid = "";

app.post("/.netlify/functions/api/getTest", (req, res) => {
    req.send("hello ma getTest")
})

app.post('/.netlify/functions/api/postData', (req, res) => {
    const { value } = req.body;
    storedPostDataValue = value;
    console.log('Received value:', value);
    res.json({ status: 'success', value: value });
});

app.get('/.netlify/functions/api/getPostData', (req, res) => {
    res.json({ value: storedPostDataValue });
});

app.post('/.netlify/functions/api/postSuhu', (req, res) => {
    console.log(req.body);
    storedSuhu = req.body;
    res.status(200).send('Data received');
});

app.get('/.netlify/functions/api/getSuhu', (req, res) => {
    res.json(storedSuhu); // Send the stored data
});

app.get('/.netlify/functions/api/getKipas', (req, res) => {
    res.json(kipasData); // Send the stored data
});

app.post('/.netlify/functions/api/postRfid', (req, res) => {
    let { id } = req.body;
    id = id.trim(); // Remove leading and trailing spaces
    uidData = id;
    console.log('Received card UID:', id);
    res.json({ status: 'success', id: id });
});

app.post('/.netlify/functions/api/hantarKipas', (req, res) => {
    console.log('Received data:', req.body);
    const { kelajuan } = req.body;
    console.log(kelajuan)
    kipasData = {
        kelajuan,
    };
    res.json({ message: 'Data updated successfully' });
});

app.post("/.netlify/functions/api/postUID", async (req, res) => {
    const { UID } = req.body; // Assuming `name` is not provided in the request body
    // Get current UTC time in milliseconds
    const nowUtc = new Date().getTime();
    // Malaysia is GMT+8, so add 8 hours in milliseconds
    const malaysiaTime = new Date(nowUtc + (3600000 * 8));
    const dateTime = malaysiaTime; // Use Malaysia time as the date

    try {
        // First, find the user by UID to get the name
        const user = await prisma.user.findUnique({
            where: {
                UID: UID,
            },
        });

        if (!user) {
            return res.status(404).json({ msg: "User not found" });
        }

        // Use the name from the found user record
        const newRecord = await prisma.entryRecord.create({
            data: {
                UID,
                name: user.name, // Use the name from the User table
                dateTime
            },
        });
        console.log(`New record created: ${JSON.stringify(newRecord)}`);
        res.status(201).json(newRecord); // Respond with the created record data
    } catch (error) {
        console.error("Error creating record:", error);
        res.status(500).json({ msg: error.message });
    }
});

app.get("/.netlify/functions/api/getEntryRecord", async (req, res) => {
    try {
        const entryRecords = await prisma.entryRecord.findMany();
        res.status(200).json(entryRecords);
    } catch (error) {
        console.error("Error fetching entry records:", error);
        res.status(500).json({ msg: error.message });
    }
});


app.get("/.netlify/functions/api/getRecord", async (req, res) => {
    console.log(`Looking for UID: ${storedUid}`); // Debug: Log the UID you're looking for
    try {
        const response = await prisma.user.findMany();
        console.log(`Response from database: ${JSON.stringify(response)}`); // Add this line before the for loop
        for (const id of response) {
            console.log(`${id.UID}`); // Debug: Log each comparison
            console.log("storedUid: " + storedUid)
            if (id.UID == storedUid) {
                console.log("Match found"); // Add this line for debugging
                return res.status(200).json("Hi " + id.name); // Exit after sending response
            }
        }
        res.status(200).json(response); // Send this response if no matching UID is found
    } catch (error) {
        res.status(500).json({ msg: error.message });
    }
});

module.exports.handler = async (event, context) => {
    const result = await handler(event, context);
    return result
}