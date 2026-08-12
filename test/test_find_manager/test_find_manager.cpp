#include <gtest/gtest.h>
#include <Arduino.h>
#include <helpers/ui/FindManager.h>
#include <helpers/ui/GenericVibration.h>

class FakeFindOutput : public FindOutput {
public:
  int starts = 0;
  int stops = 0;
  int loops = 0;

  void startFind() override { starts++; }
  void stopFind() override { stops++; }
  void loop() override { loops++; }
};

class FakeFindStateListener : public FindStateListener {
public:
  int active_events = 0;
  int inactive_events = 0;

  void onFindStateChanged(bool active) override {
    if (active) active_events++;
    else inactive_events++;
  }
};

class FindManagerTest : public testing::Test {
protected:
  void SetUp() override { resetArduinoMocks(); }
};

TEST_F(FindManagerTest, UnsupportedManagerRejectsStartAndStop) {
  FindManager manager;

  EXPECT_FALSE(manager.isSupported());
  EXPECT_FALSE(manager.start());
  EXPECT_FALSE(manager.stop());
}

TEST_F(FindManagerTest, StartsLoopsAndStopsEveryOutput) {
  FindManager manager;
  FakeFindOutput first;
  FakeFindOutput second;
  FakeFindStateListener listener;
  manager.addOutput(first);
  manager.addOutput(second);
  manager.setListener(&listener);

  EXPECT_TRUE(manager.start());
  EXPECT_TRUE(manager.isActive());
  manager.loop();
  EXPECT_TRUE(manager.stop());

  EXPECT_EQ(first.starts, 1);
  EXPECT_EQ(second.starts, 1);
  EXPECT_EQ(first.loops, 1);
  EXPECT_EQ(second.loops, 1);
  EXPECT_EQ(first.stops, 1);
  EXPECT_EQ(second.stops, 1);
  EXPECT_EQ(listener.active_events, 1);
  EXPECT_EQ(listener.inactive_events, 1);
}

TEST_F(FindManagerTest, RepeatedStartResetsDeadlineWithoutDuplicateStateEvent) {
  FindManager manager;
  FakeFindOutput output;
  FakeFindStateListener listener;
  manager.addOutput(output);
  manager.setListener(&listener);

  manager.start();
  g_mock_millis = 20000;
  manager.start();
  g_mock_millis = 49999;
  manager.loop();

  EXPECT_TRUE(manager.isActive());
  EXPECT_EQ(output.starts, 2);
  EXPECT_EQ(listener.active_events, 1);

  g_mock_millis = 50000;
  manager.loop();
  EXPECT_FALSE(manager.isActive());
  EXPECT_EQ(listener.inactive_events, 1);
}

TEST_F(FindManagerTest, StopIsIdempotentForSupportedManager) {
  FindManager manager;
  FakeFindOutput output;
  FakeFindStateListener listener;
  manager.addOutput(output);
  manager.setListener(&listener);

  EXPECT_TRUE(manager.stop());
  EXPECT_TRUE(manager.start());
  EXPECT_TRUE(manager.stop());
  EXPECT_TRUE(manager.stop());

  EXPECT_EQ(output.stops, 3);
  EXPECT_EQ(listener.active_events, 1);
  EXPECT_EQ(listener.inactive_events, 1);
}

TEST_F(FindManagerTest, TimeoutHandlesMillisRollover) {
  FindManager manager;
  FakeFindOutput output;
  manager.addOutput(output);
  g_mock_millis = 0xFFFFFFF0u;
  manager.start();

  g_mock_millis = 0x0000751Fu;
  manager.loop();
  EXPECT_TRUE(manager.isActive());

  g_mock_millis = 0x00007520u;
  manager.loop();
  EXPECT_FALSE(manager.isActive());
}

TEST_F(FindManagerTest, VibrationFindStartsImmediatelyAndStopsAtManagerTimeout) {
  FindManager manager;
  GenericVibration vibration;
  vibration.begin();
  manager.addOutput(vibration);

  EXPECT_TRUE(manager.start());
  EXPECT_TRUE(vibration.isVibrating());
  EXPECT_EQ(g_mock_digital_write_pin, PIN_VIBRATION);
  EXPECT_EQ(g_mock_digital_write_value, HIGH);

  g_mock_millis = 30000;
  manager.loop();
  EXPECT_FALSE(manager.isActive());
  EXPECT_FALSE(vibration.isVibrating());
  EXPECT_EQ(g_mock_digital_write_value, LOW);
}

TEST_F(FindManagerTest, PassiveVibrationTriggerIsIgnoredDuringFind) {
  GenericVibration vibration;
  vibration.begin();
  vibration.startFind();
  vibration.trigger();
  vibration.stopFind();

  EXPECT_FALSE(vibration.isVibrating());
  EXPECT_EQ(g_mock_digital_write_value, LOW);
}

TEST_F(FindManagerTest, VibrationFindPatternHandlesMillisRollover) {
  GenericVibration vibration;
  vibration.begin();
  g_mock_millis = 0xFFFFFFF0u;
  vibration.startFind();

  g_mock_millis = 0x000003D7u;
  vibration.loop();
  EXPECT_EQ(g_mock_digital_write_value, HIGH);

  g_mock_millis = 0x000003D8u;
  vibration.loop();
  EXPECT_EQ(g_mock_digital_write_value, LOW);
}

TEST_F(FindManagerTest, PassiveVibrationKeepsExistingTimeoutBehavior) {
  GenericVibration vibration;
  vibration.begin();
  g_mock_millis = 1;
  vibration.trigger();
  EXPECT_TRUE(vibration.isVibrating());

  g_mock_millis = VIBRATION_TIMEOUT + 2;
  vibration.loop();
  EXPECT_FALSE(vibration.isVibrating());
  EXPECT_EQ(g_mock_digital_write_value, LOW);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
