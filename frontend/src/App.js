import React, { useState, useEffect, useRef } from 'react';
import io from 'socket.io-client';
import GraphVisualization from './components/GraphVisualization';
import ChartVisualization from './components/ChartVisualization';
import ControlPanel from './components/ControlPanel';
import './App.css';

function App() {
  const [socket, setSocket] = useState(null);
  const [treeData, setTreeData] = useState(null);
  const [decisions, setDecisions] = useState([]);
  const [status, setStatus] = useState('disconnected');
  const [config, setConfig] = useState({
    numBins: 10,
    sequenceLength: 5,
    confidenceThreshold: 0.70,
    lookaheadDays: 5,
    takeProfitThreshold: 0.02
  });

  useEffect(() => {
    // Connect to Socket.io server
    const newSocket = io('http://localhost:3001');
    
    newSocket.on('connect', () => {
      console.log('Connected to server');
      setStatus('connected');
    });

    newSocket.on('disconnect', () => {
      console.log('Disconnected from server');
      setStatus('disconnected');
    });

    newSocket.on('initialized', (data) => {
      console.log('Engine initialized:', data);
      setStatus('initialized');
    });

    newSocket.on('dataLoaded', (data) => {
      console.log('Data loaded:', data);
      setStatus('data_loaded');
    });

    newSocket.on('trainComplete', (data) => {
      console.log('Training complete:', data);
      setTreeData(data.tree);
      setStatus('trained');
    });

    newSocket.on('NODE_CREATED', (node) => {
      console.log('Node created:', node);
      // Update tree data incrementally
      setTreeData(prevData => {
        // In a real implementation, you would merge the new node into the tree
        return prevData;
      });
    });

    newSocket.on('DECISION_TRIGGERED', (data) => {
      console.log('Decision triggered:', data);
      setDecisions(prev => [...prev, data]);
    });

    newSocket.on('error', (error) => {
      console.error('Server error:', error);
      alert(`Error: ${error.message}`);
    });

    setSocket(newSocket);

    return () => {
      newSocket.close();
    };
  }, []);

  const handleInitialize = () => {
    if (socket) {
      socket.emit('initialize', config);
    }
  };

  const handleLoadData = (filename) => {
    if (socket) {
      socket.emit('loadData', { filename });
    }
  };

  const handleTrain = () => {
    if (socket) {
      socket.emit('train');
    }
  };

  const handleProcessData = (data) => {
    if (socket) {
      socket.emit('processData', data);
    }
  };

  return (
    <div className="App">
      <header className="App-header">
        <h1>STDS - Sequential Trading Decision System</h1>
        <div className="status-indicator">
          Status: <span className={`status-${status}`}>{status}</span>
        </div>
      </header>
      
      <div className="main-container">
        <div className="control-section">
          <ControlPanel
            config={config}
            setConfig={setConfig}
            onInitialize={handleInitialize}
            onLoadData={handleLoadData}
            onTrain={handleTrain}
            onProcessData={handleProcessData}
            status={status}
          />
        </div>

        <div className="visualization-section">
          <div className="graph-container">
            <h2>Sequence Tree Visualization</h2>
            {treeData ? (
              <GraphVisualization data={treeData} />
            ) : (
              <div className="placeholder">
                Initialize and train the model to see the tree visualization
              </div>
            )}
          </div>

          <div className="chart-container">
            <h2>Decisions Timeline</h2>
            {decisions.length > 0 ? (
              <ChartVisualization decisions={decisions} />
            ) : (
              <div className="placeholder">
                Process data to see decisions timeline
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
