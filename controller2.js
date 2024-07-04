const express = require("express");

const app = express()
const { PrismaClient } = require("@prisma/client");

app.use(express.json());

const prisma = new PrismaClient();
let storedUid = "";

app.post("/postUID", async (req, res) => {
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

app.get("/getEntryRecord", async (req, res) => {
    try {
        const entryRecords = await prisma.entryRecord.findMany();
        res.status(200).json(entryRecords);
    } catch (error) {
        console.error("Error fetching entry records:", error);
        res.status(500).json({ msg: error.message });
    }
});


app.get("/getRecord", async (req, res) => {
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

app.listen(3002, () => {
    console.log("Server running at http://localhost:3002");
});