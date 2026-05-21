# WAVEWATCH IV Architecture

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
