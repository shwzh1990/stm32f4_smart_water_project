# Protocol
- How to bind new tank with Keypad?
  R,T(ignore),87654321,100,32,76
  | |           |        |  |  |
  | |           |        |  |  |
  | |           |        |  |  |
  | |           |        |  |  ---> signal level.
  | |           |        |  ----> ADC loadcell level.
  | |           |        -------> Battery Level
  | |            ----------------> Serial number
  | -------------------> T: Tank
  ---------------------> R: register
- How to send the tank info?
- N,T(ignore),87654321,100,32,76
    |          |       |   |  |
    |          |       |   |  |
    |          |       |   |  |
    |          |       |   |  |
    |          |       |   |  |
    |          |       |   |  ---> signal level.
    |          |       |   -----> ADC loadcell level
    |          |       ---------> Battery level
    |           -----------------> serial number
    --------------------> Tank
