// Arduino-style code for ATtiny85 Test version (using DigiSpark ATTiny85)
// PB3 = Arduino pin 3
// PB4 = Arduino pin 4

// --- Configuration Constants ---
#if defined(__AVR_ATtiny85__)
  // Settings for Digispark / ATtiny85 @ 16.5MHz
  #define PRESCALER_BITS (1 << CS01)               // Prescaler 8
  #define COMPARE_VALUE  42                        // (16.5M / (8 * 48k)) - 1
  #define TIMER_INT_MASK TIMSK                     // ATtiny85 Interrupt Mask Reg
  #define FLAG_REG TIFR
#elif defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
  // Settings for ATtiny13 @ 1.2MHz
  #define PRESCALER_BITS (1 << CS00)               // No Prescaler (1:1)
  #define COMPARE_VALUE  24                        // (1.2M / (1 * 48k)) - 1
  #define TIMER_INT_MASK TIMSK0                    // ATtiny13 Interrupt Mask Reg
  #define FLAG_REG TIFR0
#endif

// ISR(TIMER0_COMPA_vect)
// {
//   // Single-cycle hardware toggle. 
//     // This flips PB3 and PB4 at the EXACT same instant.
//     PINB = (1 << PB3) | (1 << PB4);
// }

#if defined(TIM0_COMPA_vect)
ISR(TIM0_COMPA_vect)
#else
ISR(TIMER0_COMPA_vect)
#endif
{
    // This flips PB3 and PB4 at the EXACT same instant.
    PINB = (1 << PB3) | (1 << PB4);
}

void setup()
{ 
  // --- Timer0 
  // STEP 1: IMMEDIATELY set PB3/PB4 to safe state (Low)
  PORTB &= ~((1 << PB3) | (1 << PB4)); // Set port bits low
  DDRB  |=  (1 << PB3) | (1 << PB4);  // Set as outputs

  // STEP 2: Initialize Timer0 (16.5MHz Digispark)
  TCCR0A = (1 << WGM01);   // CTC Mode
  TCCR0B = PRESCALER_BITS; // set as above
  OCR0A = COMPARE_VALUE;   // e.g. (16,500,000 / (8 * 48,000)) - 1 = 41.97
  
  // STEP 3: Clear any pending interrupts to avoid a "glitch" trigger
  FLAG_REG |= (1 << OCF0A);
  
  // 4. "PRIME" the pins for AC
  // We set PB3 HIGH just before enabling the interrupt.
  // This ensures they are opposite for the very first PINB toggle.
  PORTB |= (1 << PB3);

  // 5. Start the AC Wave
  TIMER_INT_MASK |= (1 << OCIE0A);
  
  sei(); // Start the timer logic
}

void loop()
{
    // Main loop free for other tasks
}
