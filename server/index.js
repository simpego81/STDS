const express = require('express');
const http = require('http');
const socketIO = require('socket.io');
const cors = require('cors');
const path = require('path');
const { STDSEngine } = require('../bindings/build/Release/stds_bindings.node');

const app = express();
const server = http.createServer(app);
const io = socketIO(server, {
    cors: {
        origin: "*",
        methods: ["GET", "POST"]
    }
});

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, '../frontend/build')));

// Global STDS engine instance
let engine = null;

// Socket.io connection handler
io.on('connection', (socket) => {
    console.log('Client connected:', socket.id);

    // Initialize engine
    socket.on('initialize', (config) => {
        try {
            console.log('Initializing engine with config:', config);
            
            // Create new engine with configuration
            engine = new STDSEngine(config || {
                numBins: 10,
                sequenceLength: 5,
                confidenceThreshold: 0.70,
                lookaheadDays: 5,
                takeProfitThreshold: 0.02
            });

            // Set node callback for real-time updates
            engine.setNodeCallback((node) => {
                socket.emit('NODE_CREATED', {
                    id: node.id,
                    symbol: node.symbol,
                    weight: node.weight,
                    synthesis: node.synthesis,
                    stats: node.stats
                });
            });

            socket.emit('initialized', { success: true });
        } catch (error) {
            console.error('Initialization error:', error);
            socket.emit('error', { message: error.message });
        }
    });

    // Load data
    socket.on('loadData', (data) => {
        try {
            if (!engine) {
                throw new Error('Engine not initialized');
            }

            const { filename } = data;
            const dataPath = path.join(__dirname, '../data', filename);
            
            console.log('Loading data from:', dataPath);
            const success = engine.loadData(dataPath);

            if (success) {
                socket.emit('dataLoaded', { success: true });
            } else {
                throw new Error('Failed to load data');
            }
        } catch (error) {
            console.error('Load data error:', error);
            socket.emit('error', { message: error.message });
        }
    });

    // Train model
    socket.on('train', () => {
        try {
            if (!engine) {
                throw new Error('Engine not initialized');
            }

            console.log('Training model...');
            engine.train();

            // Get tree JSON
            const treeJSON = engine.getTreeJSON();
            
            socket.emit('trainComplete', { 
                success: true,
                tree: JSON.parse(treeJSON)
            });
        } catch (error) {
            console.error('Training error:', error);
            socket.emit('error', { message: error.message });
        }
    });

    // Process new data point
    socket.on('processData', (data) => {
        try {
            if (!engine) {
                throw new Error('Engine not initialized');
            }

            const { open, high, low, close, volume } = data;
            const decision = engine.processNewData({ open, high, low, close, volume });

            socket.emit('DECISION_TRIGGERED', {
                decision,
                data: { open, high, low, close, volume },
                timestamp: Date.now()
            });
        } catch (error) {
            console.error('Process data error:', error);
            socket.emit('error', { message: error.message });
        }
    });

    // Get current tree state
    socket.on('getTree', () => {
        try {
            if (!engine) {
                throw new Error('Engine not initialized');
            }

            const treeJSON = engine.getTreeJSON();
            socket.emit('treeData', JSON.parse(treeJSON));
        } catch (error) {
            console.error('Get tree error:', error);
            socket.emit('error', { message: error.message });
        }
    });

    socket.on('disconnect', () => {
        console.log('Client disconnected:', socket.id);
    });
});

// REST API endpoints
app.get('/api/health', (req, res) => {
    res.json({ status: 'ok', timestamp: Date.now() });
});

app.post('/api/initialize', (req, res) => {
    try {
        const config = req.body;
        
        engine = new STDSEngine(config || {
            numBins: 10,
            sequenceLength: 5,
            confidenceThreshold: 0.70,
            lookaheadDays: 5,
            takeProfitThreshold: 0.02
        });

        res.json({ success: true });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.post('/api/load', (req, res) => {
    try {
        if (!engine) {
            throw new Error('Engine not initialized');
        }

        const { filename } = req.body;
        const dataPath = path.join(__dirname, '../data', filename);
        
        const success = engine.loadData(dataPath);

        if (success) {
            res.json({ success: true });
        } else {
            throw new Error('Failed to load data');
        }
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.post('/api/train', (req, res) => {
    try {
        if (!engine) {
            throw new Error('Engine not initialized');
        }

        engine.train();
        const treeJSON = engine.getTreeJSON();
        
        res.json({ 
            success: true,
            tree: JSON.parse(treeJSON)
        });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.get('/api/tree', (req, res) => {
    try {
        if (!engine) {
            throw new Error('Engine not initialized');
        }

        const treeJSON = engine.getTreeJSON();
        res.json(JSON.parse(treeJSON));
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

// Serve frontend
app.get('*', (req, res) => {
    res.sendFile(path.join(__dirname, '../frontend/build', 'index.html'));
});

const PORT = process.env.PORT || 3001;
server.listen(PORT, () => {
    console.log(`STDS Server running on port ${PORT}`);
});
