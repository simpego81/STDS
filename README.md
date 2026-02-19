# STDS - Sequential Trading Decision System

A deterministic framework for sequential trading decisions using a Suffix-like Tree structure to map market states and predict optimal trading actions.

## Overview

STDS (Sequential Trading Decision System) is a high-performance trading decision framework that:

- Maps market states into a **Suffix-like Tree** structure
- Stores historical performance (Buy, Sell, Hold) for each sequence
- Provides deterministic predictions based on pattern recognition
- Offers real-time visualization of the decision tree expansion

## Architecture

### Components

- **Backend Core**: C++11 (Memory management and intensive computation)
- **Bindings**: N-API (Node-API) for native C++/Node.js integration
- **Orchestrator**: Node.js with Express.js and Socket.io
- **Frontend**: React.js with D3.js (Graph rendering) and Lightweight Charts (OHLCV data)

### Directory Structure

```
/
├── core/                   # C++11 core logic
│   ├── include/            # Header files (.hpp)
│   ├── src/                # Implementation files (.cpp)
│   ├── lib/                # Compiled static library
│   └── CMakeLists.txt      
├── bindings/               # N-API wrapper
│   ├── src/                # Binding implementation
│   └── binding.gyp         
├── server/                 # Node.js server
│   └── index.js            
├── frontend/               # React dashboard
│   └── src/                
├── tests/                  # GTest (C++) and Jest (Node.js)
└── data/                   # CSV datasets
```

## Installation

### Prerequisites

- CMake >= 3.10
- C++11 compatible compiler (GCC, Clang)
- Node.js >= 14
- pnpm (or npm)

### Build Instructions

1. **Clone the repository**:
   ```bash
   git clone https://github.com/simpego81/STDS.git
   cd STDS
   ```

2. **Build C++ core**:
   ```bash
   cd core
   mkdir build && cd build
   cmake ..
   make
   cd ../..
   ```

3. **Build N-API bindings**:
   ```bash
   cd bindings
   pnpm install
   pnpm run build
   cd ..
   ```

4. **Install server dependencies**:
   ```bash
   cd server
   pnpm install
   cd ..
   ```

5. **Install frontend dependencies**:
   ```bash
   cd frontend
   pnpm install
   cd ..
   ```

## Usage

### Start the Server

```bash
cd server
pnpm start
```

The server will start on port 3001.

### Start the Frontend

```bash
cd frontend
pnpm start
```

The frontend will start on port 3000 and automatically open in your browser.

### Using the Application

1. **Initialize**: Configure parameters and initialize the engine
2. **Load Data**: Upload a CSV file with OHLCV data
3. **Train**: Train the model on historical data
4. **Process**: Process new data points to get trading decisions

## Configuration

### Engine Parameters

- **numBins**: Number of discrete bins for log-return quantization (default: 10)
- **sequenceLength**: Length of market state sequences (default: 5)
- **confidenceThreshold**: Threshold for decision confidence (default: 0.70)
- **lookaheadDays**: Days to look ahead for profitability check (default: 5)
- **takeProfitThreshold**: Profit threshold for signal validation (default: 0.02)

## Data Format

CSV files should have the following format:

```csv
Date,Open,High,Low,Close,Volume
2024-01-01,100.00,105.00,98.00,103.00,1000000
2024-01-02,103.00,108.00,102.00,107.00,1200000
...
```

## Testing

### C++ Tests

```bash
cd tests
mkdir build && cd build
cmake ..
make
./test_core
```

### Node.js Tests

```bash
cd tests
pnpm install
pnpm test
```

## Algorithm

### Normalization

1. Calculate log-return: `r_t = ln(C_t / C_{t-1})`
2. Map to N discrete bins
3. Output: Integer symbol identifying daily state

### Synthesis

Decision at node `n` is calculated as:

```
S(n) = {
  BUY   if buy_wins/visits > τ
  SELL  if sell_wins/visits > τ
  NONE  otherwise
}
```

Where τ is the confidence threshold (default 0.70).

## Performance

- **Training**: Processes historical data and builds the tree structure
- **Query Latency**: < 10ms per decision (as per specification)
- **Determinism**: Identical inputs produce identical outputs

## Visualization

### Tree Visualization (D3.js)

- Nodes sized by frequency (weight)
- Color coding: Green (Buy), Red (Sell), Gray (Neutral)
- Interactive tooltips with statistics

### Chart Visualization (Lightweight Charts)

- Candlestick chart with OHLCV data
- Decision markers on the chart
- Real-time decision timeline

## License

MIT License

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

## Authors

- Technical Architect: STDS Team
- Implementation: Manus AI Agent
