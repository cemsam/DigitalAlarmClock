# DigitalAlarmClock
Digital Alarm Clock on Arduino

Several functions are added in the code to perform different tasks. The code starts by
defining various variables for button pins, button states, button timers, buzzer and temperature
sensor pins and boolean variables indicating if a functionality is enabled or not such as Celsius or
Fahrenheit, AM/PM format or 24 hour format and alarm is on or off.

The timer1 of Arduino is used for timing. In setup function, TCCR1A, TCCR1B and
TIMSK1 registers are used to set up the timer and enabling timer interrupt. By enabling
corresponding bits in register, timer1 is used in CTC mode with top value OCR1A=15624. The
prescaling is set to 1024 by enabling corresponding registers in TCCR1B. OCIE1A bit is set in
TIMSK1 register in order to activate timer1 COMPA interrupt. Lastly by setting up an ISR for
timer1 COMPA, an interrupt is generated every second. This way, the seconds can be updated by
using the timer1 of Arduino.

//// SETTING UP AND USING THE CLOCK ////

A short press for button 1 changes the state of 12/24 hour mode when no setup mode is
active. If clock setup mode is active, button 1 acts as the apply button for setting up clock minute
and hour. A long press for button 1 enters the clock setup. A short press for button 2 changes the
state of alarm indicating on or off when no setup mode is active. If alarm setup mode is active,
button 2 acts as the apply button for setting up alarm minute and hour. A long press for button 2
enters the alarm setup. A short press for button 3 changes between Celsius and Fahrenheit if no
setup mode is active. If any setup mode is active the button 3 acts as the raise button. There is no
long press implemented in for this button because it is not needed. A short press for button 4
activates the snooze function. This is the only function of button 4. There is no long press
implemented for this button because it is not needed.

The default starting time for both clock and alarm are 00:00. The user can set both clock
and alarm by using the buttons as described above. When setting up the clock, a text pops up in
LCD’s first line as “C.Set” indicating that user is in clock setup mode. Also when setting up the
alarm, a text appears as “A.Set” indicating that user is in alarm setup mode. When the user exists
any setup mode, the message disappears and digital clock continues it’s normal operation. Also
when the both clock and alarm setups are set, the second variable returns to zero. So for example,
if the clock is set to 10:14 and alarm is set to 10:15, once the both setups are complete, exactly
60 seconds needs to pass in order to ring the alarm.
On top left the clock is shown, on top right there is an indication showing alarm is on or
off. On bottom left temperature is shown and on bottom right the alarm is shown.
