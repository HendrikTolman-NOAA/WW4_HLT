# WAVEWATCH IV Architecture

## Runtime Scheme Selection

WW4 uses the **Strategy** and **Factory** design patterns to enable runtime selection of numerical and physical schemes.

```mermaid
classDiagram
    class WaveModel {
        -unique_ptr~IPropagationScheme~ propagation_
        -unique_ptr~ISourceTerm~ source_
        -vector~double~ data_
        +initialize(string propName, string sourceName)
        +setData(vector~double~ initialData)
        +step()
        +getData() span~const double~
    }

    class IPropagationScheme {
        <<interface>>
        +getName() string_view
        +propagate(span~double~ data)*
    }

    class ISourceTerm {
        <<interface>>
        +getName() string_view
        +calculate(span~double~ data)*
    }

    class PropagationPr3 {
        +getName() string_view
        +propagate(span~double~ data)
    }

    class SourceSt4 {
        +getName() string_view
        +calculate(span~double~ data)
    }

    class SchemeFactory {
        +createPropagationScheme(string name) unique_ptr~IPropagationScheme~
        +createSourceTerm(string name) unique_ptr~ISourceTerm~
    }

    WaveModel o-- IPropagationScheme
    WaveModel o-- ISourceTerm
    IPropagationScheme <|-- PropagationPr3
    ISourceTerm <|-- SourceSt4
    SchemeFactory ..> IPropagationScheme : creates
    SchemeFactory ..> ISourceTerm : creates
    WaveModel ..> SchemeFactory : uses
```

### Key Components

1.  **Interfaces (`IPropagationScheme`, `ISourceTerm`)**: Define the contracts for numerical propagation and physical source term calculations. They use `std::span<double>` to process data in batches, ensuring high performance.
2.  **Concrete Schemes (`PropagationPr3`, `SourceSt4`)**: Specific implementations of the interfaces.
3.  **Scheme Factory**: A centralized registry that instantiates concrete schemes based on string identifiers at runtime.
4.  **Wave Model**: The orchestration engine that manages the lifecycle of selected schemes and executes the simulation loop.
