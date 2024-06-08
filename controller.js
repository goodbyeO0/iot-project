const express = require('express');
const fs = require('fs');
const cors = require('cors');
const bodyParser = require('body-parser');

const app = express();
app.use(cors());
app.use(bodyParser.json());
const port = 3000;
let storedValue = null;

app.get('/', (req, res) => {
    fs.readFile('./dummy.json', 'utf8', (err, data) => {
        if (err) {
            console.log(err);
            res.status(500).send('An error occurred while reading the file.');
        } else {
            res.send(JSON.parse(data));
        }
    });
});

app.post('/offOnFan', (req, res) => {
    const { value } = req.body;
    storedValue = value; // Store the value
    console.log('Received value:', value);
    res.json({ status: 'success', value: value });
});

app.get('/getData', (req, res) => {
    res.json({ value: storedValue });
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});