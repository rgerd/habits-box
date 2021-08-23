#pragma once

constexpr uint32_t NUM_SENSOR_PINS = 3;
constexpr uint32_t SENSOR_PINS[NUM_SENSOR_PINS] = { A0, A1, A2 };

// These are all per sensor.
constexpr uint32_t NUM_INPUTS = 4;
constexpr uint32_t NUM_INPUT_COMBOS = 1 << NUM_INPUTS;
// The resistors were calculated to create the largest gaps between input voltages as possible.
// The ranges were measured directly to give consistent results despite noise.
constexpr uint32_t INPUT_RANGES[NUM_INPUT_COMBOS * 2] = {
  /*0000*/ 0, 30,
  /*0001*/ 530, 550, /* 22k, ReReOr */
  /*0010*/ 1010, 1030, /* 10k, BrBlOr */
  /*0011*/ 1330, 1350,
  /*0100*/ 1680, 1700, /* 4.7k, YeViRe */
  /*0101*/ 1880, 1900,
  /*0110*/ 2070, 2090,
  /*0111*/ 2220, 2240,
  /*1000*/ 260, 280, /* 47k, YeViOr */
  /*1001*/ 730, 750,
  /*1010*/ 1170, 1190,
  /*1011*/ 1450, 1470,
  /*1100*/ 1780, 1800,
  /*1101*/ 1960, 1980,
  /*1110*/ 2150, 2170,
  /*1111*/ 2280, 2300
};

inline int _GetPressIndex(uint32_t reading) {
  for (uint32_t i = 0; i < NUM_INPUT_COMBOS; i++) {
    if (INPUT_RANGES[i * 2 + 0] < reading && reading < INPUT_RANGES[i * 2 + 1]) {
      return i;
    }
  }
  return -1;
}

inline void _GetPressString(uint32_t presses, uint32_t sensorIdx, char* buff)
{
  for (uint32_t i = 0; i < NUM_INPUTS; i++)
  {
    buff[sensorIdx * NUM_INPUTS + (NUM_INPUTS - i - 1)] = ((presses & (0b0001 << i)) >> i) + '0';
  }
}

constexpr float ANALOG_CURRENT_READING_COEFF = 0.005;
constexpr float ANALOG_AVERAGE_READING_COEFF = 1.0 - ANALOG_CURRENT_READING_COEFF;
constexpr uint32_t ANALOG_SETTLING_BIT = 1 << ((sizeof(ANALOG_SETTLING_BIT) * 8) - 1);
constexpr uint32_t ANALOG_READINGS_TO_SETTLE = 50;
struct AnalogReading
{
    AnalogReading() = default;

    inline void SetSensorPinIdx(uint32_t idx)
    {
      m_sensorPinIdx = idx;
    }

    void Update()
    {
      const auto currentReading = static_cast<float>(analogRead(SENSOR_PINS[m_sensorPinIdx]));
      m_currentAverage = (currentReading * ANALOG_CURRENT_READING_COEFF) + (m_currentAverage * ANALOG_AVERAGE_READING_COEFF);
      const auto pressIndex = _GetPressIndex(static_cast<uint32_t>(m_currentAverage));
      if (pressIndex == m_currentPressIndex) {
        if (m_currentPressIndexCount < ANALOG_READINGS_TO_SETTLE) {
          m_currentPressIndexCount++;

          if (m_currentPressIndexCount == ANALOG_READINGS_TO_SETTLE) {
            m_chosenPressIndex = m_currentPressIndex;
            // Serial.print("Current press: ");
            // Serial.println(m_chosenPressIndex, BIN);
          }
        }
      } else if (pressIndex >= 0) {
        m_currentPressIndex = pressIndex;
        m_currentPressIndexCount = 0;
        m_chosenPressIndex = ANALOG_SETTLING_BIT;
      }
    }

    inline uint32_t GetPresses() const
    {
      return m_chosenPressIndex;
    }

  private:
    uint32_t m_sensorPinIdx = ~0;
    float m_currentAverage = 0.0;
    uint32_t m_currentPressIndex = 0;
    uint32_t m_currentPressIndexCount = 0;
    uint32_t m_chosenPressIndex = ANALOG_SETTLING_BIT;
};

constexpr uint32_t LED_BUTTON_LIGHT_PIN = 2;
constexpr uint32_t LED_BUTTON_INPUT_PIN = 7;
