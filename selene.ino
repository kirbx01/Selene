const int PIEZO_PIN = 34;
const int THRESHOLD = 60;
const int BASELINE_SAMPLES = 200;
const int END_CONFIRM_COUNT = 8;
const unsigned long COOLDOWN_MS = 300;

int baseline = 0;
bool inEvent = false;
unsigned long eventStart = 0;
unsigned long lastEventEnd = 0;
int peakVal = 0;
int quietCount = 0;
int heldLevel = 0;

void setup() {
  Serial.begin(115200);
  delay(500);

  long sum = 0;
  for (int i = 0; i < BASELINE_SAMPLES; i++) {
    sum += analogRead(PIEZO_PIN);
    delay(2);
  }
  baseline = sum / BASELINE_SAMPLES;
  heldLevel = baseline;
  Serial.print("Baseline: ");
  Serial.println(baseline);
  Serial.println("READY");
}

void loop() {
  int raw = analogRead(PIEZO_PIN);

  if (raw > heldLevel) {
    heldLevel = raw;
  } else {
    heldLevel = heldLevel - 5;
    if (heldLevel < baseline) heldLevel = baseline;
  }

  int delta = abs(heldLevel - baseline);
  unsigned long now = millis();

  if (delta > THRESHOLD && !inEvent && (now - lastEventEnd > COOLDOWN_MS)) {
    inEvent = true;
    eventStart = now;
    peakVal = raw;
    quietCount = 0;
    Serial.println(">>> EVENT START");
  }

  if (inEvent) {
    if (raw > peakVal) peakVal = raw;

    if (delta < THRESHOLD / 2) {
      quietCount++;
    } else {
      quietCount = 0;
    }

    if (quietCount >= END_CONFIRM_COUNT) {
      unsigned long duration = now - eventStart;
      Serial.print(">>> EVENT END  duration_ms=");
      Serial.print(duration);
      Serial.print("  peak=");
      Serial.println(peakVal);
      inEvent = false;
      lastEventEnd = now;
    }
  }

  Serial.println(raw);
  delay(2);
}
