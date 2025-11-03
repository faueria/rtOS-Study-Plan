# rtOS Learning Plan Tailored to NASA

## The NASA Mindset

- **Correctness Over Performance**: A late answer is wrong. A wrong answer is catastrophic.
- **Determinism is Non-Negotiable**: Every operation must have a known worst-case execution time (WCET).
- **Defense in Depth**: Assume components will fail; design systems to detect, isolate, and recover from faults.
- **Verification & Testing**: Code isn't done when it works; it's done when it's proven to work under all specified conditions.

This will be a constant philosophy as we learn about and design systems doing critical work and work through the modules below.

---

## Phase 1: Foundations of Real-Time & Space Computing

**Objective**: Understand the unique constraints of space computing and the baseline concepts of real-time systems (1-2 weeks)

### Section 1.1: The Space Environment & Its Constraints

**Readings:**
- "Radiation Effects in Microelectronics"
- Concepts: Single-Event Upsets (SEU), Latch-up, Total Ionizing Dose (TID), Fault Containment Regions (FCRs).
- Space-Grade Hardware: Learn about RAD750 processors (PowerPC) and why they are used (vs. the ARM you'll use for learning).

**Key Concepts**: Reliability, Availability, Maintainability, Safety (RAMS).

### Section 1.2: Real-Time Systems Theory

**Readings:**
- *Real-Time Systems* by Jane W. S. Liu, Chapters 1-4.

**Key Concepts**: Hard vs. Firm vs. Soft Real-Time, Schedulability Analysis, Rate-Monotonic Scheduling (RMS), Earliest-Deadline First (EDF).

### Homework & Projects

**Project 1: WCET Analysis of a Simple Function.**
- Write a function to sort a small array.
- Use your MCU's cycle counter (e.g., ARM's DWT_CYCCNT) to measure its execution time.
- Run it 10,000 times with different input data to find the Best-Case Execution Time (BCET) and Worst-Case Execution Time (WCET). This is the foundational skill for schedulability analysis.

**Project 2: Schedulability Analysis.**
- Given a theoretical system with three tasks (T1: period 10ms, WCET 3ms; T2: period 20ms, WCET 5ms; T3: period 50ms, WCET 8ms), perform a Rate-Monotonic Schedulability test using the Utilization Bound Theorem. Determine if the system is schedulable.

### Self Assessment

#### Theoretical Questions:

1. **Radiation Effects**: Explain the difference between a Single-Event Upset (SEU) and latch-up. Why is one potentially recoverable while the other typically requires a power cycle? What hardware feature might you implement to protect against latch-up?

2. **Real-Time Classification**: Classify each as hard, firm, or soft real-time and justify your reasoning. A spacecraft system has three tasks:
   - Task A: Engine control loop (must complete within 2ms)
   - Task B: Science data compression (should complete within 5 seconds)
   - Task C: Telemetry downlink (important but can be delayed during high load)

3. **Scheduling Theory**: Given tasks T1(period=10ms, WCET=3ms), T2(period=20ms, WCET=5ms), T3(period=40ms, WCET=8ms):
   - Calculate total utilization using the Liu & Layland utilization bound theorem
   - Is this task set schedulable under Rate-Monotonic Scheduling?
   - What happens if T3's WCET increases to 12ms?

4. **Space-Grade Hardware**: Why would NASA choose a RAD750 processor running at 200MHz over a modern consumer-grade processor running at 2GHz for a Mars rover? List at least three reasons.

#### Practical Questions:

5. **WCET Analysis**: In your WCET measurement project:
   - What factors caused variation in execution time between different runs?
   - Why is measuring WCET more complex than just running a function many times?
   - How could compiler optimization settings affect your WCET analysis?

6. **Superloop Limitations**: Based on your cooperative scheduler project:
   - What specific symptom would indicate a task is missing its deadline?
   - How could you modify the cooperative scheduler to detect deadline misses?
   - Why does adding a third long-running task break the timing guarantees?

7. **Fault Containment**: If a radiation-induced SEU flips a single bit in your task's stack:
   - What are the potential consequences in a superloop architecture?
   - How would those consequences differ in a multitasking system?
   - What simple check could you add to detect stack corruption?

---

## Phase 2: Building a Space-Conscious Minimal Kernel

**Objective**: Implement a minimal preemptive kernel with features relevant to fault tolerance (3-5 weeks).

### Section 2.1: Kernel Internals for Reliability

**Readings:**
- *MicroC/OS-II* by Jean J. Labrosse, focusing on data structures and critical sections.

**Key Concepts**: Memory Protection (MPU), Dual-Core Lockstep, Watchdog Timers (at the task level).

### Section 2.2: Fault Tolerance Patterns

**Readings:**
- NASA Technical Reports on "Fault Detection, Isolation, and Recovery (FDIR)".

**Key Concepts**: Heartbeat Monitoring, Sanity Checks, Algorithmic Diversity (e.g., using two different algorithms to compute the same result for comparison).

### Homework & Project: "AstraOS" - A Kernel with FDIR Features

**Project 3: Preemptive Scheduler with MPU Protection**
- Implement a preemptive scheduler (as in the original plan).
- Add-on: Use the ARM Cortex-M Memory Protection Unit (MPU) to configure task stacks as read-only for other tasks. This prevents a faulty task from corrupting another task's memory. This is a simple form of a Fault Containment Region.

**Project 4: Task-Level Watchdog & Health Monitoring.**
- Implement a "Health Monitoring" task that runs at a low priority.
- Each regular task must "check in" with the Health Monitor periodically (e.g., by incrementing a counter).
- If a task fails to check in within its expected period, the Health Monitor takes a pre-defined action (e.g., resets the task, triggers a safe mode).
- This is a classic FDIR pattern used in spacecraft.

### Self Assessment

#### Theoretical Questions:

1. **Context Switching**: Describe the complete sequence of events from when SysTick fires to when a new task begins execution. Include the role of PendSV and what processor state must be saved/restored.

2. **Priority Inversion**: Explain the classic priority inversion scenario with three tasks (Low, Medium, High priority). How does a priority inheritance mutex solve this? Draw a timeline showing both the problem and solution.

3. **Synchronization Primitives**: Compare and contrast binary semaphores vs. mutexes. When would you use each? Why might a counting semaphore be dangerous in safety-critical systems?

4. **FDIR Architecture**: What is the difference between fault detection, fault isolation, and fault recovery? Give an example of each in a spacecraft attitude control system.

#### Practical Questions:

5. **MPU Protection**: In your MPU implementation:
   - What specific memory regions did you protect and why?
   - What happens if a task tries to write outside its allocated stack?
   - How does this create a Fault Containment Region?

6. **Health Monitoring**: For your task-level watchdog:
   - How did you prevent the health monitor itself from hanging?
   - What recovery actions did you implement for a failed task?
   - How could a malicious or faulty task "cheat" the heartbeat system?

7. **Critical Sections**: In your semaphore implementation:
   - How did you protect the semaphore's internal data structures?
   - What would happen if an interrupt occurred during a semaphore operation?
   - Why can't you simply use a "disable interrupts" approach for long critical sections?

8. **Resource Management**: If multiple tasks need access to a shared hardware peripheral (like I2C):
   - What synchronization mechanism would you use and why?
   - How would you prevent priority inversion in this scenario?
   - What timeout strategy would you implement to detect deadlocks?

---

## Phase 3: Professional RTOS & Space System Architecture

**Objective**: Use a professional-grade RTOS to build a complex system that mimics a spacecraft subsystem (6-8+ weeks).

### Section 3.1: Industry-Standard RTOS (FreeRTOS & RTEMS)

**Readings:**
- FreeRTOS documentation.
- RTEMS (Real-Time Executive for Multiprocessor Systems) Documentation. RTEMS is open-source and flight-proven, used in numerous spacecraft and instruments. This is your gateway to a real NASA-style RTOS.

**Key Concepts**: POSIX API (used by RTEMS), Message Queues, Event Flags.

### Section 3.2: Spacecraft Data Handling Bus (CAN & SpaceWire)

**Readings:**
- NASA GSFC "SpaceWire Primer," "CAN Bus for Aerospace" tutorials.

**Key Concepts**: Time-Triggered Architectures, Bus Arbitration, Error Detection and Correction.

### Capstone Project: Spacecraft Avionics Simulator

This project integrates all concepts. You will simulate a critical spacecraft system.

**System Architecture:**
- `Task_AttitudeControl` (High Priority): Runs a PID control loop at 100Hz. Simulates reading from a gyro and controlling reaction wheels.
- `Task_ThermalControl` (Medium Priority): Monitors "temperature sensors" (potentiometers) and controls "heaters" (LEDs) at 10Hz.
- `Task_Communications` (Low Priority): Simulates sending and receiving telemetry packets over UART (simulating a ground link).
- `Task_HealthMonitor` (Lowest Priority, but Critical): Implements the heartbeat watchdog from Project 4. Also monitors CPU load and stack usage of all tasks.

**Space-Grade Requirements:**
- **Schedulability**: You must perform a Rate-Monotonic Analysis upfront to assign priorities and prove the system is schedulable.
- **FDIR**: The Health Monitor must be able to detect if the high-priority AttitudeControl task hangs and trigger a system reset to a "Safe Mode" (all tasks stop, only a blinking LED remains).
- **Communication Integrity**: Use a Checksum or CRC in your telemetry packets between tasks and over UART.
- **Use RTEMS**: For the ultimate experience, port this project to run on RTEMS instead of FreeRTOS.

### Self Assessment

#### Theoretical Questions:

1. **RTEMS vs. FreeRTOS**: Compare the architecture and design philosophy of RTEMS vs. FreeRTOS. Why is RTEMS often preferred for flight systems despite FreeRTOS being more popular in commercial applications?

2. **SpaceWire vs. CAN**: What are the tradeoffs between SpaceWire and CAN bus for spacecraft avionics? When would you choose one over the other for inter-subsystem communication?

3. **cFS Architecture**: Explain the role of these cFS components:
   - Software Bus
   - Event Services
   - Table Services
   - How does this abstraction help with software reuse across missions?

4. **JPL Power of Ten Rules**: Select three of JPL's "Power of Ten" rules and explain how each prevents specific classes of software failures. Provide examples of bugs that each rule would catch.

#### Practical Questions:

5. **Schedulability Analysis**: For your capstone project:
   - Show your Rate-Monotonic Analysis calculations
   - What is the worst-case CPU utilization?
   - How much "slack time" do you have for future expansion?

6. **FDIR Implementation**: In your health monitoring system:
   - What symptoms can it detect beyond just task hangs?
   - How did you handle the case where the health monitor itself fails?
   - What is your strategy for escalating recovery actions (e.g., task reset vs. system safe mode)?

7. **Communication Integrity**: For your telemetry system:
   - Why did you choose your specific checksum/CRC algorithm?
   - What is the Hamming distance of your error detection code?
   - How would you handle a corrupted packet vs. a missing packet?

8. **System Validation**: How would you test these aspects of your capstone project:
   - That the attitude control task always meets its deadline
   - That the health monitor correctly detects a failed task
   - That a memory corruption in one task doesn't affect others
   - That the system recers to a safe state under power loss

9. **Lessons from Real Incidents**: Relate your capstone project design decisions to historical space incidents:
   - How does your design prevent a Mars Pathfinder-style priority inversion?
   - What features protect against an Ariane 5-style integer overflow?
   - How do you avoid the "too many files" problem that affected Mars Spirit?

---

## Beyond the 3-month Plan: The Path to NASA

1. **Study the NASA Software Safety Guidebook (NPR 7150.2)**: This is the bible. You don't need to memorize it, but understand its philosophy: rigorous requirements tracing, coding standards (like JPL's "The Power of Ten"), and extensive testing.

2. **Learn JPL's "The Power of Ten" Rules**: These are 10 rules for writing safety-critical code (e.g., no dynamic memory allocation, fixed loop bounds). Re-write your capstone project to follow these rules.

3. **Explore cFS (Core Flight System)**: This is NASA's open-source, reusable flight software framework. It runs on top of OSAL (Operating System Abstraction Layer), which abstracts RTOS like VxWorks and RTEMS. Building an application on cFS is a huge resume item.

4. **Contribute to Open-Source Space Projects**: Look for projects using RTEMS or cFS on GitHub. Even documentation contributions are a start.

This plan will not only make you proficient in RTOS but will also give you the foundational mindset and vocabulary of a spacecraft software engineer. You'll be able to speak intelligently about FDIR, schedulability, and the challenges of the space environment, setting you apart in interviews and on your path to becoming an astronaut or working with NASA. The key is to always ask, "How could this fail?" and then design against that failure.
