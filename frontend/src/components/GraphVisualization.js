import React, { useEffect, useRef } from 'react';
import * as d3 from 'd3';
import './GraphVisualization.css';

function GraphVisualization({ data }) {
  const svgRef = useRef();

  useEffect(() => {
    if (!data || !data.root) return;

    // Clear previous visualization
    d3.select(svgRef.current).selectAll('*').remove();

    const width = 800;
    const height = 600;
    const margin = { top: 20, right: 20, bottom: 20, left: 20 };

    // Create SVG
    const svg = d3.select(svgRef.current)
      .attr('width', width)
      .attr('height', height);

    // Transform tree data to D3 hierarchy
    const root = d3.hierarchy(data.root, d => {
      if (!d.children || d.children.length === 0) return null;
      return d.children;
    });

    // Create tree layout
    const treeLayout = d3.tree()
      .size([width - margin.left - margin.right, height - margin.top - margin.bottom]);

    treeLayout(root);

    // Create group for tree
    const g = svg.append('g')
      .attr('transform', `translate(${margin.left},${margin.top})`);

    // Draw links
    g.selectAll('.link')
      .data(root.links())
      .enter()
      .append('path')
      .attr('class', 'link')
      .attr('d', d3.linkVertical()
        .x(d => d.x)
        .y(d => d.y))
      .attr('fill', 'none')
      .attr('stroke', '#999')
      .attr('stroke-width', 1.5);

    // Draw nodes
    const nodes = g.selectAll('.node')
      .data(root.descendants())
      .enter()
      .append('g')
      .attr('class', 'node')
      .attr('transform', d => `translate(${d.x},${d.y})`);

    // Node circles with color based on synthesis
    nodes.append('circle')
      .attr('r', d => Math.max(5, Math.min(20, Math.sqrt(d.data.weight || 1) * 2)))
      .attr('fill', d => {
        switch (d.data.synthesis) {
          case 'BUY': return '#4CAF50';
          case 'SELL': return '#F44336';
          case 'HOLD': return '#FFC107';
          default: return '#9E9E9E';
        }
      })
      .attr('stroke', '#fff')
      .attr('stroke-width', 2);

    // Node labels
    nodes.append('text')
      .attr('dy', -25)
      .attr('text-anchor', 'middle')
      .style('font-size', '10px')
      .style('fill', '#333')
      .text(d => `S:${d.data.symbol}`);

    // Synthesis labels
    nodes.append('text')
      .attr('dy', 30)
      .attr('text-anchor', 'middle')
      .style('font-size', '9px')
      .style('font-weight', 'bold')
      .style('fill', d => {
        switch (d.data.synthesis) {
          case 'BUY': return '#4CAF50';
          case 'SELL': return '#F44336';
          case 'HOLD': return '#FFC107';
          default: return '#9E9E9E';
        }
      })
      .text(d => d.data.synthesis !== 'NONE' ? d.data.synthesis : '');

    // Add tooltip
    nodes.append('title')
      .text(d => {
        const stats = d.data.stats || {};
        return `ID: ${d.data.id}\n` +
               `Symbol: ${d.data.symbol}\n` +
               `Weight: ${d.data.weight}\n` +
               `Synthesis: ${d.data.synthesis}\n` +
               `Buy Wins: ${stats.buyWins || 0}\n` +
               `Sell Wins: ${stats.sellWins || 0}\n` +
               `Hold Count: ${stats.holdCount || 0}`;
      });

  }, [data]);

  return (
    <div className="graph-visualization">
      <svg ref={svgRef}></svg>
    </div>
  );
}

export default GraphVisualization;
