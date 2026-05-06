# WAVEWATCH IV (WW4) Code Architecture

This document provides a graphical representation of the WW4 code architecture and its components.

```mermaid
graph TD
    subgraph Repository_Root [WW4 Repository Root]
        subgraph Include_Dir [include/ww4_utils/]
            H_Const[ww4_constants.hpp]
            H_Serv[ww4_service.hpp]
            H_Time[time_management.hpp]
            H_Mem[memory_utils.hpp]
        end

        subgraph Src_Dir [src/ww4_utils/]
            S_Serv[ww4_service.cpp]
            S_Time[time_management.cpp]
            S_Mem[memory_utils.cpp]
        end

        subgraph Tests_Dir [tests_L1_L2/ww4_utils/]
            T_Const[test_ww4_constants.cpp]
            T_Serv[test_ww4_service.cpp]
            T_Time[test_time_management.cpp]
            T_Mem[test_memory_utils.cpp]
        end

        subgraph Tools_Dir [tools/]
            Tool_Setup[ww4_setup]
            Tool_Comp[ww4_compile]
            Tool_Clean[ww4_clean]
            Tool_Check[ww4_L1_L2_test_check]
        end

        subgraph Config_Templates [templates/]
            T_Config[ww4_compile_config.yml]
        end

        %% Library Relationships
        S_Serv --> H_Serv
        S_Time --> H_Time
        S_Mem --> H_Mem
        H_Serv --> H_Const

        %% Library Target
        Lib_Utils[[ww4_utils library]]
        Src_Dir --> Lib_Utils
        Include_Dir --> Lib_Utils

        %% Tests Relationships
        Tests_Dir --> Lib_Utils

        %% External Dependencies
        GTest(Google Test) -.-> Tests_Dir

        %% Tools Relationships
        Tool_Setup -.-> T_Config
        Tool_Comp -.-> Lib_Utils
        Tool_Comp -.-> Tests_Dir
        Tool_Check -.-> Src_Dir
        Tool_Check -.-> Tests_Dir
    end
```

## Component Descriptions

- **ww4_utils**: The core utility library containing mathematical constants, service routines, time management, and memory utilities.
- **include/ww4_utils/**: Public header files for the library.
- **src/ww4_utils/**: Implementation files for the library.
- **tests_L1_L2/**: Unit and integration tests for all components, using the Google Test framework.
- **tools/**: Micro-tools for project configuration, compilation, and maintenance.
- **templates/**: Configuration templates used by the setup tools.
