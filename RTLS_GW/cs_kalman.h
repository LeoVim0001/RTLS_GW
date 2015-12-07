#ifndef CS_KALMAN_H
#define CS_KALMAN_H
#include <math.h>
#include <qobject.h>

#include "armadillo"

using namespace arma;

class CS_Kalman
{
public:
    // Constructor
    CS_Kalman(void);
    void process(qint64 timeRxus, qint64 timeTxus, quint8 seqnum);
    double syncTOA(double blinkTime);

    double measNoiseVar;
    double processNoiseVar;
    double prevTxTime;
    quint8 prev_seqnum;
    bool init;
    int  id;
    // VECTOR VARIABLES
    // s.x = state vector estimate.In the input struct, this is the
    vec x;
    // s.z = observation vector
    vec z;
    // MATRIX VARIABLES :
    // s.A = state transition matrix(defaults to identity).
    mat A;
    // s.P = covariance of the state vector estimate.In the input struct,
    mat P;
    // s.Q = process noise covariance(defaults to zero).
    mat Q;
    //s.R = measurement noise covariance(required).
    mat R;
    // s.H = observation matrix(defaults to identity).
    mat H;
};

#endif // CS_KALMAN_H
