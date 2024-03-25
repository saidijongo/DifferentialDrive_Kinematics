#include <stdio.h>
#include <math.h>

#define abs(a)   (0 < a ? a : -a)

struct Encoder {
    float _tics;
    Encoder (int a, int b)  { }
    void set  (float tics)  { _tics = tics; }
    int  read (void)        { return  _tics; }
};

struct Serial {
    void begin (int )       { }

    void print (const char *s)  { printf ("%s", s); }
    void print (int i)          { printf ("%d", i); }
    void println (int i)        { printf ("%d\n", i); }
} Serial;

unsigned long millis (void)  {
    static unsigned long msec;

    return msec += 10;
}

// -----------------------------------------------------------------------------
Encoder Left(3, 2);
Encoder Right(11, 10);

float R = 385; //mm
float r = 12.5;
float L = 8;
double LeftDist = 0;
long LOldPos  = -999;
float LNewPos;
int Correction;
double RightDist = 0;
long ROldPos  = -999;
float RNewPos;
int fi = 0;
unsigned long t;
int Xt = 0;
int Xtb = 0;
int Yt = 0;
int Ytb = 0;
int fit = 0;
int fitb = 0;
int Vicc;

// ---------------------------------------------------------
void setup() {
    Serial.begin(9600);
    t = millis();
}

// ---------------------------------------------------------
//0.9mm/1 deg
void Encoders() {
    LNewPos = Left.read();
    if (LNewPos != LOldPos) {
        LOldPos  = LNewPos;
        LeftDist = ((LNewPos / 840) * (2 * r * 3.14)) / 10; //MM / Rev
    }
    //-----------------------------------------------------
    RNewPos = Right.read();
    if (RNewPos  != ROldPos) {
        ROldPos   = RNewPos;
        RightDist = ((RNewPos / 840) * (2 * r * 3.14)) / 10; //MM / Rev
    }

    fi = (abs(RightDist - LeftDist) * 0.9) - Correction;

    if (fi >= 360)
        Correction += 360;
    else if (fi <= -360)
        Correction -= 360;
}

// ---------------------------------------------------------
void loop() {
    Encoders();
    if ((millis() - t) >= 100) {
        Xt   = Xtb;
        Yt   = Ytb;
        fit  = fitb;
        Vicc = (L * (LeftDist / (RightDist / LeftDist))) + 0.5;
        Xtb  = Xt + (Vicc * (sin(fit + fi) - sin (fit)));
        Ytb  = Yt + (Vicc * (cos(fit) - cos(fit + fi)));
        fitb = fit + fi;
        t = millis();
    }
    Serial.print(Vicc);
    Serial.print("        ");
    Serial.print(Xtb);
    Serial.print("        ");
    Serial.println(Ytb);
}

// -----------------------------------------------------------------------------
const int   TicPerRot = 800;
const int   WheelDia  = 2;                    // inches
const float TicPerIn  = TicPerRot / (M_PI * WheelDia);

const int   Rad       = 24;                   // inches
const float CircOuter = M_TWOPI * (Rad + 2);  // in.
const float CircInner = M_TWOPI * (Rad - 2);  // in.

/*
 dL = circL * k = TwoPi * TicPerIn * fracCirc * (rad + 2)
 dR = circR * k = TwoPi * TicPerIn * fracCirc * (rad - 2)

 dL - dR = TwoPi * TicPerIn * fracCirc * 4

 fracCirc = (dL - dR) / (TwoPi * TicPerIn * 4)

 dL + dR = TwoPi * TicPerIn * fracCirc * ((rad + 2) - (rad - 2))
 dL + dR = TwoPi * TicPerIn * fracCirc * 2 * rad

 rad  = (dL + dR) / (TwoPi * TicPerIn * fracCirc * 2)

 dist    = (dL + dR) / 2

 */

int posL;
int posR;

void
analyze (void)
{
    int pos = Left.read ();
    int dL  = pos - posL;
    posL = pos;

    pos     = Right.read ();
    int dR  = pos - posR;
    posR = pos;

    float fracCirc  = (dL - dR) / (M_TWOPI * TicPerIn * 4);
    float rad       = (dL + dR) / (M_TWOPI * TicPerIn * fracCirc * 2);

    printf (" %6.4f fracCirc", fracCirc);
    printf (" %6.1f dAngle", fracCirc * 360);
    printf (" %6.1f rad", rad);
    printf ("\n");
}

// -------------------------------------
int
main ()
{
    printf (" %6d TicPerRot\n", TicPerRot);
    printf (" %6d WheelDia\n",  WheelDia);
    printf (" %6.1f TicPerIn\n",  TicPerIn);
    printf (" %6d Rad\n",       Rad);
    printf (" %6.1f CircOuter\n", CircOuter);
    printf (" %6.1f CircInner\n", CircInner);

    int dA  = 15;
    for (int a = 0; a <= 360; a += dA)  {
        Left.set  (TicPerIn * CircOuter * a / 360);
        Right.set (TicPerIn * CircInner * a / 360);

        printf (" a %3d", a);
        printf (", L %5d", Left.read ());
        printf (", R %5d - ", Right.read ());
#if 0
        loop ();
#else
        analyze ();
#endif
    }

    return 0;
}
