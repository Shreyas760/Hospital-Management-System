const express = require('express');
const { exec } = require('child_process');
const util = require('util');
const cors = require('cors');
const app = express();

// Promisify exec for async/await
const execPromise = util.promisify(exec);

// Middleware
app.use(cors());
app.use(express.json());

// Helper function to escape command-line arguments
function escapeArg(arg) {
    return `"${arg.replace(/"/g, '\\"')}"`;
}

// API Endpoints
app.post('/api/patients', async (req, res) => {
    const { id, name, age, contact, bloodGroup, reason } = req.body;
    const command = `hms.exe add_patient ${id} ${escapeArg(name)} ${age} ${escapeArg(contact)} ${escapeArg(bloodGroup)} ${escapeArg(reason)}`;
    try {
        const { stdout } = await execPromise(command);
        res.json(JSON.parse(stdout));
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.get('/api/patients', async (req, res) => {
    try {
        const { stdout } = await execPromise('hms.exe view_patients');
        res.json(JSON.parse(stdout));
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.get('/api/patients/sorted', async (req, res) => {
    try {
        const { stdout } = await execPromise('hms.exe view_sorted_patients');
        res.json(JSON.parse(stdout));
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.put('/api/patients/:id', async (req, res) => {
    const id = req.params.id;
    const { name, age } = req.body;
    const command = `hms.exe update_patient ${id} ${escapeArg(name)} ${age}`;
    try {
        const { stdout } = await execPromise(command);
        res.json(JSON.parse(stdout));
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.delete('/api/patients/:id', async (req, res) => {
    const id = req.params.id;
    try {
        const { stdout } = await execPromise(`hms.exe delete_patient ${id}`);
        res.json(JSON.parse(stdout));
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.post('/api/doctors', async (req, res) => {
    const { id, name, contact, specialty } = req.body;
    const command = `hms.exe add_doctor ${id} ${escapeArg(name)} ${escapeArg(contact)} ${escapeArg(specialty)}`;
    try {
        const { stdout } = await execPromise(command);
        res.json(JSON.parse(stdout));
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.get('/api/doctors', async (req, res) => {
    try {
        const { stdout } = await execPromise('hms.exe view_doctors');
        res.json(JSON.parse(stdout));
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.get('/api/doctors/recommend', async (req, res) => {
    const reason = req.query.reason;
    const command = `hms.exe recommend_doctor ${escapeArg(reason)}`;
    try {
        const { stdout } = await execPromise(command);
        res.json(JSON.parse(stdout));
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.delete('/api/doctors/:id', async (req, res) => {
    const id = req.params.id;
    try {
        const { stdout } = await execPromise(`hms.exe delete_doctor ${id}`);
        res.json(JSON.parse(stdout));
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.post('/api/undo', async (req, res) => {
    try {
        const { stdout } = await execPromise('hms.exe undo');
        res.json(JSON.parse(stdout));
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

const path = require('path');

// Serve static files
app.use(express.static(path.join(__dirname)));

// Serve index.html for root
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'index.html'));
});

const API_URL = '/api';

// Start server
app.listen(3000, () => {
    console.log('Server running on http://localhost:3000');
});