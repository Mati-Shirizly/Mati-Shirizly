# Medical Device MVP Workflow Diagrams

## Overview Timeline (Gantt Chart)

```mermaid
gantt
    title Medical Device MVP Development Timeline
    dateFormat YYYY-MM-DD
    section Phase 1: Foundation
    Regulatory Strategy           :p1a, 2025-01-01, 3w
    Requirements Definition       :p1b, 2025-01-01, 3w
    Risk Management              :p1c, 2025-01-01, 3w
    Architecture Design          :p1d, 2025-01-01, 3w

    section Phase 2: Hardware
    Proof of Concept            :p2a, 2025-01-22, 3w
    Electronic Design           :p2b, 2025-02-12, 3w
    Firmware Development        :p2c, 2025-02-12, 4w
    Mechanical Design           :p2d, 2025-03-05, 2w
    Initial Testing             :p2e, 2025-03-19, 2w

    section Phase 3: Mobile App
    Setup & Foundation          :p3a, 2025-01-22, 2w
    Core Features Sprint 1      :p3b, 2025-02-05, 2w
    Core Features Sprint 2      :p3c, 2025-02-19, 2w
    Integration                 :p3d, 2025-03-05, 2w
    Security Implementation     :p3e, 2025-03-19, 1w
    Testing                     :p3f, 2025-03-26, 1w

    section Phase 4: Dashboard
    Setup & Foundation          :p4a, 2025-02-05, 2w
    Core Features Sprint 1      :p4b, 2025-02-19, 2w
    Core Features Sprint 2      :p4c, 2025-03-05, 2w
    Analytics & Reporting       :p4d, 2025-03-19, 2w
    Integration                 :p4e, 2025-04-02, 1w
    Testing                     :p4f, 2025-04-09, 1w

    section Phase 5: Backend
    Backend Development         :p5a, 2025-01-22, 3w
    Security Implementation     :p5b, 2025-02-12, 2w
    Compliance Infrastructure   :p5c, 2025-02-26, 2w
    DevOps & Monitoring        :p5d, 2025-03-12, 2w

    section Phase 6: Integration
    System Integration          :p6a, 2025-04-02, 2w
    Clinical Validation         :p6b, 2025-04-16, 2w
    Regulatory Testing          :p6c, 2025-04-16, 2w
    Security Audit              :p6d, 2025-04-30, 1w
    Documentation               :p6e, 2025-04-30, 1w
```

## System Architecture Flow

```mermaid
flowchart TB
    subgraph Hardware["Hardware Layer"]
        US[Ultrasound Probe]
        UT[Ultrasound Transducer]
        SP[Signal Processor]
        FW[Firmware]

        UT --> SP
        SP --> FW
        FW --> US
    end

    subgraph Mobile["Mobile Application Layer"]
        MA[Mobile App]
        RT[Real-time Display]
        PM[Patient Management]
        IM[Image Storage]
        MS[Measurements]

        MA --> RT
        MA --> PM
        MA --> IM
        MA --> MS
    end

    subgraph Backend["Backend Layer"]
        API[REST API]
        AUTH[Authentication]
        DB[(Database)]
        FS[File Storage]
        WS[WebSocket Server]

        API --> DB
        API --> FS
        AUTH --> API
        WS --> API
    end

    subgraph Dashboard["Dashboard Layer"]
        DA[Dashboard App]
        DM[Device Management]
        AN[Analytics]
        RV[Report Viewer]
        UM[User Management]

        DA --> DM
        DA --> AN
        DA --> RV
        DA --> UM
    end

    subgraph Cloud["Cloud Infrastructure"]
        CDN[CDN]
        LB[Load Balancer]
        MON[Monitoring]
        LOG[Logging]
    end

    US -->|USB-C/BLE| MA
    MA -->|HTTPS| API
    DA -->|HTTPS| API
    API --> LB
    LB --> MON
    LB --> LOG
    FS --> CDN

    style Hardware fill:#e1f5ff
    style Mobile fill:#fff4e1
    style Backend fill:#e8f5e9
    style Dashboard fill:#f3e5f5
    style Cloud fill:#fce4ec
```

## Phase Dependencies

```mermaid
flowchart LR
    P1[Phase 1:<br/>Foundation &<br/>Planning]
    P2[Phase 2:<br/>Hardware<br/>Development]
    P3[Phase 3:<br/>Mobile App<br/>Development]
    P4[Phase 4:<br/>Dashboard<br/>Development]
    P5[Phase 5:<br/>Backend &<br/>Infrastructure]
    P6[Phase 6:<br/>Integration &<br/>Validation]

    P1 --> P2
    P1 --> P3
    P1 --> P4
    P1 --> P5
    P2 --> P6
    P3 --> P6
    P4 --> P6
    P5 --> P3
    P5 --> P4
    P5 --> P6

    style P1 fill:#ff6b6b
    style P2 fill:#4ecdc4
    style P3 fill:#45b7d1
    style P4 fill:#96ceb4
    style P5 fill:#ffeaa7
    style P6 fill:#dfe6e9
```

## Development Workflow Process

```mermaid
flowchart TD
    START([Start MVP Development])

    REQ[Define Requirements<br/>& Specifications]
    REG[Regulatory Strategy<br/>& Planning]
    ARCH[System Architecture<br/>Design]
    RISK[Risk Management<br/>Plan]

    START --> REQ
    START --> REG
    REQ --> ARCH
    REG --> ARCH
    ARCH --> RISK

    subgraph Parallel["Parallel Development Streams"]
        HW[Hardware Development]
        MOB[Mobile Development]
        DASH[Dashboard Development]
        BE[Backend Development]
    end

    RISK --> Parallel

    HW --> PROTO[Hardware Prototype]
    MOB --> ALPHA_M[Mobile Alpha]
    DASH --> ALPHA_D[Dashboard Alpha]
    BE --> API_V1[API v1]

    PROTO --> INT
    ALPHA_M --> INT
    ALPHA_D --> INT
    API_V1 --> INT

    INT[System Integration<br/>Testing]

    INT --> VAL[Clinical Validation]
    INT --> SEC[Security Audit]
    INT --> REG_TEST[Regulatory Testing]

    VAL --> DOC
    SEC --> DOC
    REG_TEST --> DOC

    DOC[Documentation<br/>Completion]

    DOC --> SUB[Regulatory<br/>Submission]
    SUB --> DECISION{Approval?}

    DECISION -->|Yes| LAUNCH[MVP Launch]
    DECISION -->|No| REVISE[Revisions Required]
    REVISE --> INT

    LAUNCH --> END([Production Ready])

    style START fill:#95e1d3
    style END fill:#95e1d3
    style DECISION fill:#f38181
    style LAUNCH fill:#aa96da
```

## Data Flow Diagram

```mermaid
flowchart LR
    subgraph User["User Interactions"]
        CLI[Clinician]
        ADM[Admin]
        PAT[Patient Data]
    end

    subgraph Device["Ultrasound Probe"]
        IMG[Raw Image Data]
        META[Device Metadata]
    end

    subgraph Mobile["Mobile App"]
        PROC[Image Processing]
        MEAS[Measurements]
        ANNOT[Annotations]
        LOC_DB[(Local Storage)]
    end

    subgraph Cloud["Cloud Backend"]
        API_GW[API Gateway]
        AUTH_SVC[Auth Service]
        IMG_SVC[Image Service]
        PAT_SVC[Patient Service]
        DEV_SVC[Device Service]
        S3[(Object Storage)]
        DB[(PostgreSQL)]
    end

    subgraph Dashboard["Web Dashboard"]
        VIEWER[Image Viewer]
        REPORTS[Reports]
        ANALYTICS[Analytics]
        ADMIN[Admin Panel]
    end

    CLI -->|Scans Patient| Device
    Device -->|USB-C/BLE| Mobile
    IMG --> PROC
    META --> LOC_DB
    PROC --> MEAS
    MEAS --> ANNOT

    Mobile -->|HTTPS/REST| API_GW
    API_GW --> AUTH_SVC
    API_GW --> IMG_SVC
    API_GW --> PAT_SVC
    API_GW --> DEV_SVC

    IMG_SVC --> S3
    PAT_SVC --> DB
    DEV_SVC --> DB
    PAT --> PAT_SVC

    Dashboard -->|HTTPS/REST| API_GW
    S3 --> VIEWER
    DB --> REPORTS
    DB --> ANALYTICS
    ADM --> ADMIN

    style User fill:#ffe66d
    style Device fill:#4ecdc4
    style Mobile fill:#45b7d1
    style Cloud fill:#96ceb4
    style Dashboard fill:#ff6b6b
```

## Risk Management Flow

```mermaid
flowchart TD
    IDENTIFY[Identify Hazards]

    IDENTIFY --> ANALYZE[Analyze Risks]
    ANALYZE --> EVAL[Evaluate Risk Level]

    EVAL --> DECISION{Acceptable?}

    DECISION -->|Yes| MONITOR[Monitor Risk]
    DECISION -->|No| CONTROL[Implement Controls]

    CONTROL --> VERIFY[Verify Effectiveness]
    VERIFY --> REEVAL[Re-evaluate Risk]

    REEVAL --> DECISION

    MONITOR --> REVIEW[Periodic Review]
    REVIEW --> UPDATE{Changes?}

    UPDATE -->|Yes| IDENTIFY
    UPDATE -->|No| MONITOR

    subgraph Controls["Risk Controls"]
        DESIGN[Design Controls]
        PROC[Process Controls]
        INFO[Information/Training]
        SOFT[Software Safeguards]
    end

    CONTROL -.-> Controls

    style DECISION fill:#f38181
    style UPDATE fill:#f38181
    style MONITOR fill:#95e1d3
```

## Testing & Validation Pyramid

```mermaid
flowchart BT
    subgraph L1["Unit Tests"]
        UT1[Hardware Unit Tests]
        UT2[Software Unit Tests]
        UT3[Firmware Unit Tests]
    end

    subgraph L2["Integration Tests"]
        IT1[Hardware-Software Integration]
        IT2[API Integration]
        IT3[Component Integration]
    end

    subgraph L3["System Tests"]
        ST1[End-to-End Testing]
        ST2[Performance Testing]
        ST3[Security Testing]
    end

    subgraph L4["Validation"]
        VAL1[Clinical Validation]
        VAL2[Usability Testing]
        VAL3[Regulatory Testing]
    end

    L1 --> L2
    L2 --> L3
    L3 --> L4

    style L1 fill:#e8f5e9
    style L2 fill:#fff9c4
    style L3 fill:#ffe0b2
    style L4 fill:#f8bbd0
```

## Regulatory Pathway

```mermaid
flowchart TD
    START([Project Initiation])

    START --> CLASS[Determine Device<br/>Classification]

    CLASS --> PATH{Select Pathway}

    PATH -->|US| FDA[FDA Pathway]
    PATH -->|EU| CE[CE Marking]
    PATH -->|Other| OTHER[Other Regions]

    FDA --> FDA_CLASS{Classification}
    FDA_CLASS -->|Class I| FDA_C1[General Controls]
    FDA_CLASS -->|Class II| FDA_510K[510k Submission]
    FDA_CLASS -->|Class III| FDA_PMA[PMA Submission]

    CE --> CE_CLASS{Classification}
    CE_CLASS -->|Class I| CE_C1[Self Declaration]
    CE_CLASS -->|Class IIa/IIb| CE_NB[Notified Body Review]
    CE_CLASS -->|Class III| CE_C3[Full Assessment]

    FDA_C1 --> FDA_REG[FDA Registration]
    FDA_510K --> FDA_REV[FDA Review]
    FDA_PMA --> FDA_REV

    CE_C1 --> CE_DOC[Technical Documentation]
    CE_NB --> CE_DOC
    CE_C3 --> CE_DOC

    FDA_REV --> FDA_DEC{Decision}
    FDA_DEC -->|Approved| MARKET_US[US Market Launch]
    FDA_DEC -->|More Info| FDA_RESP[Respond to Questions]
    FDA_DEC -->|Not Approved| FDA_APPEAL[Appeal/Revise]

    FDA_RESP --> FDA_REV
    FDA_APPEAL --> FDA_510K

    CE_DOC --> CE_AUDIT[Notified Body Audit]
    CE_AUDIT --> CE_DEC{Decision}
    CE_DEC -->|Approved| CERT[CE Certificate]
    CE_DEC -->|CAR| CE_CAR[Corrective Actions]

    CE_CAR --> CE_AUDIT
    CERT --> MARKET_EU[EU Market Launch]

    style START fill:#95e1d3
    style MARKET_US fill:#aa96da
    style MARKET_EU fill:#aa96da
    style FDA_DEC fill:#f38181
    style CE_DEC fill:#f38181
```

## Sprint Structure (Agile)

```mermaid
gantt
    title 2-Week Sprint Cycle
    dateFormat YYYY-MM-DD

    section Sprint Planning
    Sprint Planning Meeting    :milestone, m1, 2025-01-06, 0d

    section Development
    Design & Architecture      :d1, 2025-01-06, 2d
    Implementation            :d2, after d1, 6d
    Code Review               :d3, after d2, 1d

    section Testing
    Unit Testing              :t1, 2025-01-08, 5d
    Integration Testing       :t2, after d2, 2d

    section Documentation
    Technical Docs            :doc1, 2025-01-13, 2d

    section Sprint Close
    Sprint Review             :milestone, m2, 2025-01-17, 0d
    Sprint Retrospective      :milestone, m3, 2025-01-17, 0d

    section Next Sprint
    Sprint Planning           :milestone, m4, 2025-01-20, 0d
```

---

## How to View These Diagrams

### Option 1: GitHub
- Push this file to GitHub and view it there (GitHub renders Mermaid)

### Option 2: VS Code
- Install "Markdown Preview Mermaid Support" extension
- Open this file and use preview (Cmd+Shift+V)

### Option 3: Online Viewers
- Copy diagram code to: https://mermaid.live/
- Or use: https://mermaid.ink/

### Option 4: Generate Images
```bash
# Install mermaid-cli
npm install -g @mermaid-js/mermaid-cli

# Generate PNG images
mmdc -i workflow-diagram.md -o workflow-diagram.png
```
