# FreeRTOS Sensor Monitor

ARM Cortex-M3 기반 실시간 센서 모니터링 펌웨어.  
하드웨어 없이 **QEMU**로 실행 가능한 임베디드 포트폴리오 프로젝트.

---

## 프로젝트 개요

| 항목 | 내용 |
|------|------|
| RTOS | FreeRTOS v11 |
| 타겟 | ARM Cortex-M3 (LM3S6965) |
| 에뮬레이터 | QEMU `lm3s6965evb` |
| 빌드 시스템 | CMake + arm-none-eabi-gcc |

---

## 아키텍처

```
┌──────────────────────────────────────────────────────┐
│                     FreeRTOS Scheduler                │
├──────────────┬────────────────┬───────────┬──────────┤
│ SensorTask   │  ProcessTask   │ UARTTask  │ Watchdog │
│ (Priority 3) │  (Priority 2)  │(Priority1)│(Prior. 4)│
│              │                │           │          │
│ 500ms 주기로  │  임계값 초과 시  │  Mutex로  │ 태스크   │
│ 센서 읽기    │  WARN 레벨 설정  │ UART 출력 │ 생존 감시│
└──────┬───────┴───────┬────────┴─────┬─────┴──────────┘
       │               │              │
       ▼               ▼              │
  [SensorQueue]   [LogQueue]    [heartbeat kick]
  (depth: 8)      (depth: 16)
```

### 태스크 설명

| 태스크 | 우선순위 | 역할 |
|--------|----------|------|
| **SensorTask** | 3 | 500ms 주기로 가상 센서(온도/습도/기압) 읽기 → SensorQueue 전송 |
| **ProcessTask** | 2 | SensorQueue 수신 → 임계값 비교 → 포맷 후 LogQueue 전송 |
| **UARTTask** | 1 | LogQueue 수신 → Mutex 보호 하에 UART 출력 |
| **WatchdogTask** | 4 | 2초 주기로 모든 태스크 heartbeat 확인, 3초 이상 응답 없으면 경고 |

### 동기화 구조

- `SensorQueue` — SensorTask → ProcessTask 데이터 전달 (Queue)
- `LogQueue` — ProcessTask → UARTTask 로그 전달 (Queue)
- `uart_mutex` — UART 접근 보호 (Mutex), UARTTask / WatchdogTask 공유
- `watchdog_kick()` — 각 태스크가 주기적으로 호출하는 heartbeat 함수

---

## 빌드 및 실행

### 의존성 설치

**macOS**
```bash
brew install --cask gcc-arm-embedded
brew install cmake qemu
```

**Ubuntu**
```bash
sudo apt install gcc-arm-none-eabi cmake qemu-system-arm
```

### 클론 및 빌드

```bash
git clone --recursive https://github.com/MinwooPyeon/freertos-sensor-monitor.git
cd freertos-sensor-monitor

chmod +x scripts/build.sh scripts/run_qemu.sh
./scripts/build.sh
```

### QEMU 실행

```bash
./scripts/run_qemu.sh
```

**예상 출력:**
```
[INFO] [    0 ms] TEMP: 25.3 C  HUM: 61.2%  PRES:1013 hPa
[INFO] [  500 ms] TEMP: 26.1 C  HUM: 58.7%  PRES:1015 hPa
[WARN] [ 1000 ms] TEMP: 28.5 C  HUM: 76.3%  PRES:1012 hPa
[WATCHDOG] All tasks healthy at 2000 ms
```

종료: `Ctrl+A` → `X`

---

## 디버깅 (GDB)

```bash
# 터미널 1 — QEMU를 GDB 대기 모드로 실행
qemu-system-arm -machine lm3s6965evb -cpu cortex-m3 \
    -kernel build/freertos-sensor-monitor.elf \
    -serial mon:stdio -nographic -S -gdb tcp::1234

# 터미널 2 — GDB 연결
arm-none-eabi-gdb build/freertos-sensor-monitor.elf \
    -ex "target remote :1234" \
    -ex "break main" \
    -ex "continue"
```

---

## 파일 구조

```
freertos-sensor-monitor/
├── CMakeLists.txt
├── FreeRTOS-Kernel/          # git submodule
├── scripts/
│   ├── build.sh
│   └── run_qemu.sh
└── src/
    ├── main.c
    ├── config/
    │   └── FreeRTOSConfig.h
    ├── drivers/
    │   ├── uart_driver.c/h   # 레지스터 직접 제어 UART 드라이버
    │   └── virtual_sensor.c/h
    ├── startup/
    │   ├── startup_lm3s6965.c  # 벡터 테이블, Reset_Handler
    │   └── lm3s6965.ld         # 링커 스크립트
    └── tasks/
        ├── sensor_task.c/h
        ├── process_task.c/h
        ├── uart_task.c/h
        └── watchdog_task.c/h
```

---

## 핵심 학습 포인트

- **선점형 RTOS 스케줄링** — 우선순위 기반 태스크 전환 이해
- **태스크 간 통신** — Queue를 통한 데이터 파이프라인 설계
- **동기화** — Mutex를 이용한 공유 자원(UART) 보호
- **저수준 드라이버** — UART 레지스터 직접 제어 (IBRD/FBRD/LCRH/CTL)
- **Watchdog 패턴** — 실시간 시스템의 장애 감지 메커니즘
- **링커 스크립트 / 스타트업 코드** — .data copy, .bss zero-fill, 벡터 테이블
