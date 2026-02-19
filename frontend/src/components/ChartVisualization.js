import React, { useEffect, useRef } from 'react';
import { createChart } from 'lightweight-charts';
import './ChartVisualization.css';

function ChartVisualization({ decisions }) {
  const chartContainerRef = useRef();
  const chartRef = useRef();

  useEffect(() => {
    if (!chartContainerRef.current) return;

    // Create chart
    const chart = createChart(chartContainerRef.current, {
      width: chartContainerRef.current.clientWidth,
      height: 400,
      layout: {
        background: { color: '#ffffff' },
        textColor: '#333',
      },
      grid: {
        vertLines: { color: '#e1e1e1' },
        horzLines: { color: '#e1e1e1' },
      },
      timeScale: {
        timeVisible: true,
        secondsVisible: true,
      },
    });

    chartRef.current = chart;

    // Add candlestick series
    const candlestickSeries = chart.addCandlestickSeries({
      upColor: '#4CAF50',
      downColor: '#F44336',
      borderVisible: false,
      wickUpColor: '#4CAF50',
      wickDownColor: '#F44336',
    });

    // Transform decisions to candlestick data
    const candlestickData = decisions.map(decision => ({
      time: Math.floor(decision.timestamp / 1000),
      open: decision.data.open,
      high: decision.data.high,
      low: decision.data.low,
      close: decision.data.close,
    }));

    candlestickSeries.setData(candlestickData);

    // Add markers for decisions
    const markers = decisions.map(decision => {
      let color, position, shape, text;
      
      switch (decision.decision) {
        case 'BUY':
          color = '#4CAF50';
          position = 'belowBar';
          shape = 'arrowUp';
          text = 'BUY';
          break;
        case 'SELL':
          color = '#F44336';
          position = 'aboveBar';
          shape = 'arrowDown';
          text = 'SELL';
          break;
        case 'HOLD':
          color = '#FFC107';
          position = 'inBar';
          shape = 'circle';
          text = 'HOLD';
          break;
        default:
          return null;
      }

      return {
        time: Math.floor(decision.timestamp / 1000),
        position,
        color,
        shape,
        text,
      };
    }).filter(m => m !== null);

    candlestickSeries.setMarkers(markers);

    // Handle resize
    const handleResize = () => {
      if (chartContainerRef.current && chartRef.current) {
        chartRef.current.applyOptions({
          width: chartContainerRef.current.clientWidth,
        });
      }
    };

    window.addEventListener('resize', handleResize);

    return () => {
      window.removeEventListener('resize', handleResize);
      if (chartRef.current) {
        chartRef.current.remove();
      }
    };
  }, [decisions]);

  return (
    <div className="chart-visualization">
      <div ref={chartContainerRef} className="chart-container-inner"></div>
      <div className="decisions-list">
        <h3>Recent Decisions</h3>
        <ul>
          {decisions.slice(-10).reverse().map((decision, idx) => (
            <li key={idx} className={`decision-${decision.decision.toLowerCase()}`}>
              <span className="decision-time">
                {new Date(decision.timestamp).toLocaleTimeString()}
              </span>
              <span className="decision-type">{decision.decision}</span>
              <span className="decision-price">
                ${decision.data.close.toFixed(2)}
              </span>
            </li>
          ))}
        </ul>
      </div>
    </div>
  );
}

export default ChartVisualization;
