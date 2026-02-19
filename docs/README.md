# STDS Documentation

This directory contains comprehensive documentation for the Sequential Trading Decision System (STDS) framework.

## Documentation Files

### UML Documentation

The **[UML_DOCUMENTATION.md](UML_DOCUMENTATION.md)** file provides complete architectural and design documentation through UML diagrams:

- **Use Case Diagram**: Functional requirements and actor interactions
- **Class Diagram**: Static structure of the C++ core layer
- **Component Diagram**: High-level architecture across all layers
- **Deployment Diagram**: Physical deployment and runtime environment
- **Sequence Diagrams**: Step-by-step workflows for training and decision making
- **State Diagram**: Engine lifecycle and state transitions
- **Activity Diagram**: Detailed training process with decision logic

All diagrams are available in both PlantUML source format (`.puml`) and rendered PNG images in the `uml/` directory.

## UML Diagrams Overview

### Architecture Diagrams

These diagrams illustrate the overall system structure and component relationships:

1. **Component Diagram** - Shows the layered architecture from frontend to core
2. **Deployment Diagram** - Depicts the physical deployment across client and server machines
3. **Class Diagram** - Details the C++ core classes and their relationships

### Behavioral Diagrams

These diagrams describe the dynamic behavior and workflows of the system:

1. **Use Case Diagram** - Captures functional requirements from user perspective
2. **Sequence Diagrams** - Illustrate message flows for training and decision making
3. **State Diagram** - Models the engine lifecycle and state transitions
4. **Activity Diagram** - Shows the detailed training process flow

## Quick Links

- [Main README](../README.md) - Project overview and setup instructions
- [Implementation Summary](../IMPLEMENTATION_SUMMARY.md) - Detailed implementation report
- [UML Documentation](UML_DOCUMENTATION.md) - Complete UML diagram documentation
- [UML Source Files](uml/) - PlantUML source files for all diagrams

## Using the Documentation

### For Developers

The UML diagrams serve as a design reference and implementation guide. The class diagram provides the blueprint for the C++ core, while sequence diagrams detail the interaction patterns between components.

### For Architects

The component and deployment diagrams offer a high-level view of the system architecture, technology stack, and deployment strategy. These are useful for understanding the overall design decisions and scalability considerations.

### For Users

The use case diagram explains what the system can do and how different actors interact with it. The state diagram helps understand the system's operational modes and valid transitions.

### For Maintainers

All diagrams are maintained in PlantUML format, making them easy to update as the system evolves. The source files are version-controlled alongside the code to ensure documentation stays in sync with implementation.

## Modifying Diagrams

To modify any UML diagram:

1. Edit the corresponding `.puml` file in the `uml/` directory
2. Regenerate the PNG image:
   ```bash
   cd docs/uml
   manus-render-diagram <diagram-name>.puml <diagram-name>.png
   ```
3. Commit both the source and rendered image

## Diagram Standards

All UML diagrams follow these standards:

- **Format**: PlantUML syntax for source files
- **Rendering**: PNG images at high resolution
- **Naming**: Descriptive names with hyphens (e.g., `use-case-diagram.puml`)
- **Documentation**: Each diagram is documented in UML_DOCUMENTATION.md
- **Versioning**: Both source and rendered images are version-controlled

## Additional Resources

- [PlantUML Documentation](https://plantuml.com/)
- [UML 2.5 Specification](https://www.omg.org/spec/UML/)
- [C4 Model](https://c4model.com/) - Inspiration for component diagrams

---

**Last Updated**: February 19, 2026  
**Maintained by**: STDS Development Team
