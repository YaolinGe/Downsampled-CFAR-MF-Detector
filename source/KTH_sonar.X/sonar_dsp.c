#include <math.h>
#include "downlink.h"
#include "sonar_dps.h"

// Correlate 2 values through mult(s1*,s2)
int32c correlate16(int16c s1,int16c s2){
    int32c out;
    out.re = s2.re*s1.re+s2.im*s1.im;
    out.im = -s2.re*s1.im+s2.im*s1.re;
    return out;
}


/* Reference: "Numerical Recipes in C" 2nd Ed.
 * by W.H.Press, S.A.Teukolsky, W.T.Vetterling, B.P.Flannery
 * (1992) Cambridge University Press.
 * ISBN 0-521-43108-5
 * Sec.13.4 - Data Windowing
 */
double parzen(int i, int nn) {
    return (1.0 - fabs(((double) i - 0.5 * (double) (nn - 1))
            / (0.5 * (double) (nn + 1))));
}

double welch(int i, int nn) {
    return (1.0 - (((double) i - 0.5 * (double) (nn - 1))
            / (0.5 * (double) (nn + 1)))
            *(((double) i - 0.5 * (double) (nn - 1))
            / (0.5 * (double) (nn + 1))));
}

double hanning(int i, int nn) {
    return ( 0.5 * (1.0 - cos(2.0 * M_PI * (double) i / (double) (nn - 1))));
}

/* Reference: "Digital Filters and Signal Processing" 2nd Ed.
 * by L. B. Jackson. (1989) Kluwer Academic Publishers.
 * ISBN 0-89838-276-9
 * Sec.7.3 - Windows in Spectrum Analysis
 */
double hamming(int i, int nn) {
    return ( 0.54 - 0.46 * cos(2.0 * M_PI * (double) i / (double) (nn - 1)));
}

double blackman(int i, int nn) {
    return ( 0.42 - 0.5 * cos(2.0 * M_PI * (double) i / (double) (nn - 1))
            + 0.08 * cos(4.0 * M_PI * (double) i / (double) (nn - 1)));
}

double steeper(int i, int nn) {
    return ( 0.375
            - 0.5 * cos(2.0 * M_PI * (double) i / (double) (nn - 1))
            + 0.125 * cos(4.0 * M_PI * (double) i / (double) (nn - 1)));
}

/* apply window function to data[]
 * INPUT
 *  flag_window : 0 : no-window (default -- that is, other than 1 ~ 6)
 *                1 : parzen window
 *                2 : welch window
 *                3 : hanning window
 *                4 : hamming window
 *                5 : blackman window
 *                6 : steeper 30-dB/octave rolloff window
 */
void windowing(int n, const double *data, int flag_window, double scale,
        double *out) {
    int i;
    for (i = 0; i < n; i++) {
        switch (flag_window) {
            case 1: // parzen window
                out [i] = data [i] * parzen(i, n) / scale;
                break;

            case 2: // welch window
                out [i] = data [i] * welch(i, n) / scale;
                break;

            case 3: // hanning window
                out [i] = data [i] * hanning(i, n) / scale;
                break;

            case 4: // hamming window
                out [i] = data [i] * hamming(i, n) / scale;
                break;

            case 5: // blackman window
                out [i] = data [i] * blackman(i, n) / scale;
                break;

            case 6: // steeper 30-dB/octave rolloff window
                out [i] = data [i] * steeper(i, n) / scale;
                break;

            default:
                printString("Invalid flag window function");
                printEL();
            case 0: // square (no window)
                out [i] = data [i] / scale;
                break;
        }
    }
}