class ClockState {
  protected:
    int lastNtpUpdate;
    int colourScheme;

public:
  /// create an led controller object, add it to the chain of controllers
    ClockState() : lastNtpUpdate(-1), colourScheme(0) {
    }

    void setNtpUpdate(int datetime);

