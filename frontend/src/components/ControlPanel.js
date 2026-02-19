import React, { useState } from 'react';
import './ControlPanel.css';

function ControlPanel({ config, setConfig, onInitialize, onLoadData, onTrain, onProcessData, status }) {
  const [filename, setFilename] = useState('sample.csv');
  const [manualData, setManualData] = useState({
    open: 100,
    high: 105,
    low: 98,
    close: 103,
    volume: 1000000
  });

  const handleConfigChange = (key, value) => {
    setConfig(prev => ({
      ...prev,
      [key]: parseFloat(value) || value
    }));
  };

  const handleManualDataChange = (key, value) => {
    setManualData(prev => ({
      ...prev,
      [key]: parseFloat(value)
    }));
  };

  return (
    <div className="control-panel">
      <h2>Control Panel</h2>
      
      <div className="config-section">
        <h3>Configuration</h3>
        <div className="config-item">
          <label>Number of Bins:</label>
          <input
            type="number"
            value={config.numBins}
            onChange={(e) => handleConfigChange('numBins', e.target.value)}
          />
        </div>
        <div className="config-item">
          <label>Sequence Length:</label>
          <input
            type="number"
            value={config.sequenceLength}
            onChange={(e) => handleConfigChange('sequenceLength', e.target.value)}
          />
        </div>
        <div className="config-item">
          <label>Confidence Threshold:</label>
          <input
            type="number"
            step="0.01"
            value={config.confidenceThreshold}
            onChange={(e) => handleConfigChange('confidenceThreshold', e.target.value)}
          />
        </div>
        <div className="config-item">
          <label>Lookahead Days:</label>
          <input
            type="number"
            value={config.lookaheadDays}
            onChange={(e) => handleConfigChange('lookaheadDays', e.target.value)}
          />
        </div>
        <div className="config-item">
          <label>Take Profit Threshold:</label>
          <input
            type="number"
            step="0.01"
            value={config.takeProfitThreshold}
            onChange={(e) => handleConfigChange('takeProfitThreshold', e.target.value)}
          />
        </div>
        <button 
          onClick={onInitialize}
          disabled={status !== 'connected' && status !== 'initialized'}
        >
          Initialize Engine
        </button>
      </div>

      <div className="data-section">
        <h3>Load Data</h3>
        <div className="config-item">
          <label>CSV Filename:</label>
          <input
            type="text"
            value={filename}
            onChange={(e) => setFilename(e.target.value)}
          />
        </div>
        <button 
          onClick={() => onLoadData(filename)}
          disabled={status !== 'initialized' && status !== 'data_loaded'}
        >
          Load Data
        </button>
      </div>

      <div className="train-section">
        <h3>Training</h3>
        <button 
          onClick={onTrain}
          disabled={status !== 'data_loaded' && status !== 'trained'}
        >
          Train Model
        </button>
      </div>

      <div className="process-section">
        <h3>Process New Data</h3>
        <div className="config-item">
          <label>Open:</label>
          <input
            type="number"
            step="0.01"
            value={manualData.open}
            onChange={(e) => handleManualDataChange('open', e.target.value)}
          />
        </div>
        <div className="config-item">
          <label>High:</label>
          <input
            type="number"
            step="0.01"
            value={manualData.high}
            onChange={(e) => handleManualDataChange('high', e.target.value)}
          />
        </div>
        <div className="config-item">
          <label>Low:</label>
          <input
            type="number"
            step="0.01"
            value={manualData.low}
            onChange={(e) => handleManualDataChange('low', e.target.value)}
          />
        </div>
        <div className="config-item">
          <label>Close:</label>
          <input
            type="number"
            step="0.01"
            value={manualData.close}
            onChange={(e) => handleManualDataChange('close', e.target.value)}
          />
        </div>
        <div className="config-item">
          <label>Volume:</label>
          <input
            type="number"
            value={manualData.volume}
            onChange={(e) => handleManualDataChange('volume', e.target.value)}
          />
        </div>
        <button 
          onClick={() => onProcessData(manualData)}
          disabled={status !== 'trained'}
        >
          Process Data
        </button>
      </div>
    </div>
  );
}

export default ControlPanel;
