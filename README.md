# FreeRTOS Sensor Monitor

ARM Cortex-M3 기반 실시간 센서 모니터링 펌웨어. 하드웨어 없이 **QEMU**로 실행 가능한 임베디드 포트폴리오 프로젝트.

| RTOS | 타겟 | 에뮬레이터 | 빌드 |
|------|------|-----------|------|
| FreeRTOS v11 | ARM Cortex-M3 (LM3S6965) | QEMU `lm3s6965evb` | CMake + arm-none-eabi-gcc |

---

## 아키텍처

```
[SensorTask P3] --(SensorQueue)--> [ProcessTask P2] --(LogQueue)--> [UARTTask P1]
                                                                          |
[WatchdogTask P4] <-------------- heartbeat kick -------------------------+
```

| 태스크 | 우선순위 | 역할 |
|--------|----------|------|
| SensorTask | 3 | 500ms 주기로 온도/습도/기압 읽기 → SensorQueue |
| ProcessTask | 2 | 임계값 비교 후 로그 포맷 → LogQueue |
| UARTTask | 1 | Mutex 보호 하에 UART 출력 |
| WatchdogTask | 4 | 태스크 heartbeat 감시, 3초 무응답 시 경고 |

---

## 빌드 및 실행

**의존성 설치**

```bash
# macOS
brew install --cask gcc-arm-embedded && brew install cmake qemu

# Ubuntu
sudo apt install gcc-arm-none-eabi cmake qemu-system-arm
```

**빌드 및 실행**

```bash
git clone --recursive https://github.com/MinwooPyeon/freertos-sensor-monitor.git
cd freertos-sensor-monitor
chmod +x scripts/*.sh
./scripts/build.sh
./scripts/run_qemu.sh
```

**예상 출력**
```
[INFO] [    0 ms] TEMP: 25.3 C  HUM: 61.2%  PRES:1013 hPa
[WARN] [ 1000 ms] TEMP: 28.5 C  HUM: 76.3%  PRES:1012 hPa
[WATCHDOG] All tasks healthy at 2000 ms
```

종료: `Ctrl+A` → `X`

---

## GDB 디버깅

```bash
# 터미널 1
qemu-system-arm -machine lm3s6965evb -cpu cortex-m3 \
    -kernel build/freertos-sensor-monitor.elf \
    -serial mon:stdio -nographic -S -gdb tcp::1234

# 터미널 2
arm-none-eabi-gdb build/freertos-sensor-monitor.elf \
    -ex "target remote :1234" -ex "break main" -ex "continue"
```

---

## 핵심 구현 포인트

- 선점형 RTOS 스케줄링 (우선순위 기반 태스크 전환)
- Queue 기반 태스크 간 데이터 파이프라인
- Mutex를 이용한 공유 자원(UART) 동기화
- UART0 레지스터 직접 제어 드라이버 (IBRD/FBRD/LCRH/CTL)
- Watchdog heartbeat 패턴으로 태스크 장애 감지
- 링커 스크립트 + 스타트업 코드 (벡터 테이블, .data/.bss 초기화)
