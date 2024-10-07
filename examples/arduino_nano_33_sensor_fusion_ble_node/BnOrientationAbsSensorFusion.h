/**
* MIT License
* 
* Copyright (c) 2024 Manuel Bottini
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef BN_ORIENTATION_ABS_SENSOR_FUSION_H
#define BN_ORIENTATION_ABS_SENSOR_FUSION_H

#include "stdio.h"
#include <string.h>
#include <cstdint>
#include <cstdlib>

class BnMatrix {
public:

    // Copy constructor
    BnMatrix(const BnMatrix& other);
    // Copy assignment operator
    BnMatrix& operator=(const BnMatrix& other);
    // Move constructor
    BnMatrix(BnMatrix&& other) noexcept;
    // Move assignment constuctor
    BnMatrix& operator=(BnMatrix&& other) noexcept;

    BnMatrix(const uint16_t rows, const uint16_t columns );
    BnMatrix(const uint16_t rows, const uint16_t columns, const float *values );
    ~BnMatrix();

    float val(const uint16_t row, const uint16_t column) const;
    void val(const uint16_t row, const uint16_t column, float value);

    bool isEmpty() const;
    uint16_t rows() const;
    uint16_t columns() const;
    void print() const;
    BnMatrix transposed();
    float determinant() const;
    BnMatrix cofactor();
    void multiply(const float mult);
    BnMatrix inverted();

    static void multiply(BnMatrix &result, const BnMatrix &matrL, const BnMatrix &matrR );
    static BnMatrix multiply( const BnMatrix &matrL, const BnMatrix &matrR );
    static void sum(BnMatrix &result, const BnMatrix &matrL, const BnMatrix &matrR );
    static BnMatrix sum( const BnMatrix &matrL, const BnMatrix &matrR );
    static void subtract(BnMatrix &result, const BnMatrix &matrL, const BnMatrix &matrR );
    static BnMatrix subtract( const BnMatrix &matrL, const BnMatrix &matrR );
    static bool equal( const BnMatrix &matrL, const BnMatrix &matrR );
    static BnMatrix identity( const uint16_t size );

protected:

    // I could pass a reference and make values a nullptr on return.
    // But I don't know if anyone will just save the pointer and the reassing it.
    // Just bare in mind that once you call bindValues you don't need to free values
    // Note: values must be dynamically allocated
    void bindValues(float *values);

    // Somehow previous implementations were creating problems
    // in the creation of the dll library (windows). So I decided to
    // create member functions that play with direct arrays instead
    // of doing calling other member functions. But then I understood
    // the problem and solved. Since these functions are more performant
    // I will just keep them
    static float determinantArray(const uint16_t rows, const uint16_t columns, const float *values_orig);
    static float* cofactorArray(const uint16_t rows, const uint16_t columns, const float *values_orig);
    static float* transposeArray(const uint16_t rows, const uint16_t columns, const float *values_orig);
    static void multiplyInplaceArray(const uint16_t rows, const uint16_t columns, float *values_orig, const float mult);

    uint16_t m_rows = 0;
    uint16_t m_columns = 0;
    float* m_values = nullptr;
    bool m_isEmpty = true;
};


class BnVector : public BnMatrix {

public:

    BnVector(const uint16_t rows) : BnMatrix(rows, 1 ) {}
    BnVector(const uint16_t rows, const float *values) : BnMatrix(rows, 1, values) {}
    ~BnVector() {}

    // Copy constructor from BnMatrix
    BnVector(const BnMatrix& other);
    // Copy assignment operator from BnMatrix
    BnVector& operator=(const BnMatrix& other);

    BnVector transpose() = delete;

    void normalize();

    static void productElementwise(BnVector &result, const BnVector &vecL, const BnVector &vecR );
    static BnVector productElementwise(const BnVector &vecL, const BnVector &vecR  );


};

class BnEulerAngles;

class BnQuaternion : public BnVector {

public:

    BnQuaternion() : BnVector(4) {}

    BnQuaternion(const float w_v,const float x_v,const float y_v, const float z_v) : BnVector(4) {
        w( w_v);
        x( x_v);
        y( y_v);
        z( z_v);
    }

    // Copy constructor from BnMatrix
    BnQuaternion(const BnMatrix& other);
    // Copy assignment operator from BnMatrix
    BnQuaternion& operator=(const BnMatrix& other);

    void getRotationMatrix(BnMatrix &matrO);
    void getEulerAngles(BnEulerAngles &eulerAnglesO);
    void conjugate();

    float w() const;
    float x() const;
    float y() const;
    float z() const;
    void w(const float value);
    void x(const float value);
    void y(const float value);
    void z(const float value);

    static void productHamilton(BnQuaternion &result, const BnQuaternion &quatL, const BnQuaternion &quatR );
    static BnQuaternion productHamilton( const BnQuaternion &quatL, const BnQuaternion &quatR );

};


class BnEulerAngles : public BnVector {

public:

    BnEulerAngles() : BnVector(3) {}

    BnEulerAngles(float roll_rads, float pitch_rads, float yaw_rads) : BnVector(3) {
        roll(roll_rads);
        pitch(pitch_rads);
        yaw(yaw_rads);
    }

    void getRotationMatrix(BnMatrix &matrO);
    void getQuaternion(BnQuaternion &quaternionO);

    float roll() const;
    float yaw() const;
    float pitch() const;
    void roll(const float value);
    void yaw(const float value);
    void pitch(const float value);

    void printRadiants() const;
    void printDegrees() const;

    static float toDegrees(const float rads);
    static float toRadiants(const float degrees);

};

class BnSensorFusionMadgwickAHRS {

public:

    BnSensorFusionMadgwickAHRS(
        const uint32_t samplePeriod_ms,
        const float gain,
        const float rescaleGyro,
        const BnVector &axisSigns);

    void init(const BnQuaternion &initialQuat);

    void updateIMU(
        const BnVector &gyro,
        const BnVector &accel,
        const uint64_t time_now = 0);
    void updateMAGR(
        const BnVector &gyro,
        const BnVector &accel,
        const BnVector &magn,
        const uint64_t time_now = 0);
    void getQuaternion(BnQuaternion &out);

private:

    void updateSamplePeriod_ms(const uint64_t time_now);

    uint32_t m_samplePeriod_ms;
    float m_gain;
    float m_rescaleGyro;
    BnVector m_axisSigns;
    BnQuaternion m_internalQuat;
    uint64_t m_timeNow;

};

#include "BnNodeSpecific.h"
#include "BnDatatypes.h"



class BnOrientationAbsSensor {
public:

  BnOrientationAbsSensor();
  void init();
  bool checkAllOk();
  bool isCalibrated();
  BnSensorData getData();
  String getType();
  void setEnable(bool enable_status);
  bool isEnabled();

private:
  void setStatus(int sensor_status);
  void realignAxis(float values[], float revalues[]);

  bool s_enabled;
  bool s_sensorInit;
  BnStatusLED s_statusSensorLED;
  unsigned long s_lastReadSensorTime;
  unsigned long s_sensorReconnectionTime;
  float s_values[4];
  BnSensorFusionMadgwickAHRS s_sensorfusion;

};

#ifdef BODYNODE_GLOVE_SENSOR

class BnGloveSensorReaderSerial {
public:
  // Initializes the reader
  void init();
  // Reads from the serial. Returns true if a full read has been received, false otherwise.
  bool checkAllOk();
  // Returns the data read
  void getData(int *values);
  // Returns the type of the sensor as string
  String getType();
  // Enable/Disable Sensor
  void setEnable(bool enable_status);
  // Returns if sensor is enabled or not
  bool isEnabled();

private:
  boolean grs_lineDone;
  String grs_lineToPrint;
  bool grs_enabled;
};

#endif /*BODYNODE_GLOVE_SENSOR*/

#ifdef BODYNODE_SHOE_SENSOR

class BnShoeSensor {
public:
  // Initializes the reader
  void init();
  // Reads from the serial. Returns true if a full read has been received, false otherwise.
  bool checkAllOk();
  // Returns the data read
  void getData(int *values);
  // Returns the type of the sensor as string
  String getType();
  // Enable/Disable Sensor
  void setEnable(bool enable_status);
  // Returns if sensor is enabled or not
  bool isEnabled();

private:
  int ss_value;
  int ss_pin;
  bool ss_enabled;
};

#endif /*BODYNODE_SHOE_SENSOR*/

#endif // BN_ORIENTATION_ABS_SENSOR_FUSION_H
