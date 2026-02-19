const { STDSEngine } = require('../bindings/build/Release/stds_bindings.node');
const path = require('path');

describe('STDS Bindings Tests', () => {
  let engine;

  beforeEach(() => {
    engine = new STDSEngine({
      numBins: 10,
      sequenceLength: 5,
      confidenceThreshold: 0.70,
      lookaheadDays: 5,
      takeProfitThreshold: 0.02
    });
  });

  test('Engine initialization', () => {
    expect(engine).toBeDefined();
  });

  test('Load data', () => {
    const dataPath = path.join(__dirname, '../data/sample.csv');
    const result = engine.loadData(dataPath);
    expect(result).toBe(true);
  });

  test('Train model', () => {
    const dataPath = path.join(__dirname, '../data/sample.csv');
    engine.loadData(dataPath);
    
    expect(() => {
      engine.train();
    }).not.toThrow();
  });

  test('Get tree JSON', () => {
    const dataPath = path.join(__dirname, '../data/sample.csv');
    engine.loadData(dataPath);
    engine.train();
    
    const treeJSON = engine.getTreeJSON();
    expect(treeJSON).toBeDefined();
    expect(typeof treeJSON).toBe('string');
    
    const tree = JSON.parse(treeJSON);
    expect(tree).toHaveProperty('root');
  });

  test('Process new data', () => {
    const dataPath = path.join(__dirname, '../data/sample.csv');
    engine.loadData(dataPath);
    engine.train();
    
    const newData = {
      open: 120.0,
      high: 125.0,
      low: 119.0,
      close: 123.0,
      volume: 1500000
    };
    
    const decision = engine.processNewData(newData);
    expect(decision).toBeDefined();
    expect(typeof decision).toBe('string');
    expect(['BUY', 'SELL', 'HOLD', 'NONE']).toContain(decision);
  });

  test('Node callback', (done) => {
    let callbackCalled = false;
    
    engine.setNodeCallback((node) => {
      callbackCalled = true;
      expect(node).toHaveProperty('id');
      expect(node).toHaveProperty('symbol');
      expect(node).toHaveProperty('weight');
      expect(node).toHaveProperty('synthesis');
      expect(node).toHaveProperty('stats');
    });
    
    const dataPath = path.join(__dirname, '../data/sample.csv');
    engine.loadData(dataPath);
    engine.train();
    
    // Give some time for callbacks to be processed
    setTimeout(() => {
      expect(callbackCalled).toBe(true);
      done();
    }, 100);
  });
});

describe('API Latency Tests', () => {
  test('Training latency should be reasonable', () => {
    const engine = new STDSEngine();
    const dataPath = path.join(__dirname, '../data/sample.csv');
    engine.loadData(dataPath);
    
    const startTime = Date.now();
    engine.train();
    const endTime = Date.now();
    
    const latency = endTime - startTime;
    console.log(`Training latency: ${latency}ms`);
    
    // Training should complete in reasonable time
    expect(latency).toBeLessThan(5000);
  });

  test('Query latency should be under 10ms', () => {
    const engine = new STDSEngine();
    const dataPath = path.join(__dirname, '../data/sample.csv');
    engine.loadData(dataPath);
    engine.train();
    
    const newData = {
      open: 120.0,
      high: 125.0,
      low: 119.0,
      close: 123.0,
      volume: 1500000
    };
    
    const startTime = Date.now();
    engine.processNewData(newData);
    const endTime = Date.now();
    
    const latency = endTime - startTime;
    console.log(`Query latency: ${latency}ms`);
    
    // Query should be very fast
    expect(latency).toBeLessThan(10);
  });
});
