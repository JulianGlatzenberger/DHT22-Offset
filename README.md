# DHT22-Offset
Calculate non-linear Regression Offset for DHT22 sensors using arduino.

## Wiring:


## Usage Guide:
1. Run arduino code this will save the raw temperature from the sensors onto the SD-Card.
2. Measure the actual temperature manually.
3. Enter both the real temperature values and the calculated offsets into the Python script: `T = np.array([7.9,19.1, 22.1])` and `O = np.array([1.9,2.7, 1.1])`
4. Execute [non-linear-regression.py](/non-linear-regression.py). This will generate a plot and display the calculated regression coefficients in the console.

5. Paste the coefficients into the [arduino-file](/Calculate_DHT22_Offset/Calculate_DHT22_Offset.ino) at: <br />
```
const float a = -0.00219;
const float b = 0.00935;
const float c = 1.96278;
```

**Note: Keep the sensor read interval as short as possible to minimize self-heating and ensure accurate measurements.**
