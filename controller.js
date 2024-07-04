const express = require('express');
const fs = require('fs');
const cors = require('cors');
const bodyParser = require('body-parser');
// const { PrismaClient } = require("@prisma/client");
// const ServerlessHttp = require('serverless-http');

// const prisma = new PrismaClient();
const app = express();
app.use(cors());
app.use(express.json())
// const handler = ServerlessHttp(app)
let storedPostDataValue = null;
let storedValue = null;
let storedSuhu = null;
let kipasData = {
    kelajuan: 150, // Set initial value to 150
};
let uidData = null;



app.get('/.netlify/functions/api', (req, res) => {
    fs.readFile('../dummy.json', 'utf8', (err, data) => {
        if (err) {
            console.log(err);
            res.status(500).send('An error occurred while reading the file.');
        } else {
            res.send(JSON.parse(data));
        }
    });
});

app.post('/.netlify/functions/api/offOnFan', (req, res) => {
    const { value } = req.body;
    storedValue = value; // Store the value
    console.log('Received value:', value);
    res.json({ status: 'success', value: value });
});

app.get('/.netlify/functions/api/getData', (req, res) => {
    res.json({ value: storedValue });
});

app.post('/postData', (req, res) => {
    const { value } = req.body;
    storedPostDataValue = value;
    console.log('Received value:', value);
    res.json({ status: 'success', value: value });
});

app.get('/getPostData', (req, res) => {
    res.json({ value: storedPostDataValue });
});

app.post('/postSuhu', (req, res) => {
    console.log(req.body);
    storedSuhu = req.body;
    res.status(200).send('Data received');
});

app.get('/getSuhu', (req, res) => {
    res.json(storedSuhu); // Send the stored data
});




app.get('/getKipas', (req, res) => {
    res.json(kipasData); // Send the stored data
});

app.post('/postRfid', (req, res) => {
    let { id } = req.body;
    id = id.trim(); // Remove leading and trailing spaces
    uidData = id;
    console.log('Received card UID:', id);
    res.json({ status: 'success', id: id });
});

app.get('/getUID', (req, res) => {
    res.json(uidData); // Send the stored data
});

// app.get("/record", async (req, res) => {
//     try {
//         const response = await prisma.record.findMany();
//         response.forEach((id) => {
//             console.log(uidData)
//             console.log(id.UID)
//             console.log(id.name)
//             if (id.UID == uidData) console.log(`Hi ${id.name}`)
//         })
//         res.status(200).json(response);
//     } catch (error) {
//         res.status(500).json({ msg: error.message });
//     }
// })
app.post('/hantarKipas', (req, res) => {
    console.log('Received data:', req.body);
    const { kelajuan } = req.body;
    console.log(kelajuan)
    kipasData = {
        kelajuan,
    };
    res.json({ message: 'Data updated successfully' });
});

app.listen(3000, () => {
    console.log("Server running at htt http://localhost:3000")
})

// module.exports.handler = async (event, context) => {
//     const result = await handler(event, context);
//     return result
// }