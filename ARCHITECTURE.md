# WAVEWATCH IV Architecture

### Key Components

1.  **ISolver**: The primary integration strategy. It defines the contract for numerical solvers that combine dynamics (propagation) and physics (source terms) to advance the model state.
2.  **ISourceTerm**: The physical strategy interface. Implementations (e.g., `SourceSt4`) are injected into the solver.
3.  **SolverPr3**: A concrete solver that manages a collection of source terms and executes them during its `solve` phase.
4.  **SchemeFactory**: Responsible for instantiating individual model components (solvers and source terms).
5.  **WaveModel**: The high-level orchestrator. It manages the simulation loop and triggers the configured solver at each time step.
This document describes the high-level architecture of WAVEWATCH IV (WW4) using a Mermaid diagram.

## Component Diagram

```mermaid
graph TD
    subgraph "Programs"
        SA[ww4_standalone]
    end

    subgraph "Core Library"
        CORE[ww4_core]
    end

    subgraph "Utility Library"
        UTILS[ww4_utils]
    end

    subgraph "Tests"
        L1L2[tests]
    end

    subgraph "External Dependencies"
        YAML[yaml-cpp]
        GTEST[googletest]
    end

    SA --> CORE
    SA --> UTILS
    CORE --> UTILS

    L1L2 --> CORE
    L1L2 --> UTILS
    L1L2 --> GTEST

    UTILS --> YAML
```

## Call Structure

The following diagram illustrates the sequence of function calls when running the `ww4_standalone` program.

```mermaid
sequenceDiagram
    participant SA as ww4_standalone
    participant INIT as w4core_init
    participant WAVE as w4core_wave
    participant FINL as w4core_finalize
    participant UTILS as ww4_utils

    SA->>UTILS: loadStandaloneConfig()
    SA->>INIT: w4core_init(startTime)
    INIT->>UTILS: loadRunConfig()
    INIT->>UTILS: setCalendarType()
    INIT->>UTILS: ww4_input_update()
    INIT->>UTILS: writeInitialOutput()
    INIT->>UTILS: writeLogTableHeader()

    SA->>WAVE: w4core_wave(startTime, endTime)
    loop until endTime
        WAVE->>UTILS: assessOutputConfig()
        WAVE->>UTILS: updateAllInputs()
        WAVE->>UTILS: computeInputTimeStep()
        WAVE->>UTILS: computeOutputTimeStep()
        WAVE->>UTILS: updateOutputActualTimes()
        Note over WAVE: Propagate Solution
    end

    SA->>FINL: w4core_finalize(endTime)
```

## Description of Components

- **Programs**: Contains end-user applications. `ww4_standalone` is a simplified environment for running the wave model core.
- **Core Library (`ww4_core`)**: Implements the main wave model routines, including initialization (`w4core_init`), time stepping (`w4core_wave`), and finalization (`w4core_finalize`).
- **Utility Library (`ww4_utils`)**: Provides common functionality such as time management, configuration loading, logging, and standard output utilities.
- **Tests**: Contains unit tests (Level 1, prefixed with `L1_test_`) and integration tests (Level 2, prefixed with `L2_test_`) using the Google Test framework.
- **External Dependencies**:
    - `yaml-cpp`: Used for parsing YAML configuration files.
    - `googletest`: Used for unit and integration testing.

## Integrated Runtime Solver Selection

WW4 uses an integrated **Strategy** and **Factory** design pattern. To support complex interleaving of physics and dynamics (e.g., implicit sub-stepping), physical source terms are managed and called directly by a **Solver**.

```mermaid
classDiagram
    class WaveModel {
        -unique_ptr~ISolver~ solver_
        -vector~double~ data_
        +initialize(unique_ptr~ISolver~ solver)
        +setData(vector~double~ initialData)
        +step()
        +getData() span~const double~
    }

    class ISolver {
        <<interface>>
        +getName() string_view
        +addSourceTerm(unique_ptr~ISourceTerm~ source)*
        +solve(span~double~ data)*
    }

    class ISourceTerm {
        <<interface>>
        +getName() string_view
        +calculate(span~double~ data)*
    }

    class SolverPr3 {
        -vector~unique_ptr~ISourceTerm~~ sourceTerms_
        +getName() string_view
        +addSourceTerm(unique_ptr~ISourceTerm~ source)
        +solve(span~double~ data)
    }

    class SourceSt4 {
        +getName() string_view
        +calculate(span~double~ data)
    }

    class SchemeFactory {
        +createSolver(string name) unique_ptr~ISolver~
        +createSourceTerm(string name) unique_ptr~ISourceTerm~
    }

    WaveModel o-- ISolver : orchestrates
    ISolver <|-- SolverPr3
    SolverPr3 o-- ISourceTerm : manages & calls
    ISourceTerm <|-- SourceSt4
    SchemeFactory ..> ISolver : creates
    SchemeFactory ..> ISourceTerm : creates
```