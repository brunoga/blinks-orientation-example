#include "src/blinks-orientation/orientation.h"

static bool was_alone_;

void setup() {}

void loop() {
  if (isAlone()) {
    // We are alone, so record that information for later check.
    was_alone_ = true;
  } else if (was_alone_) {
    // We were alone but now we are connected. Record that and setup our
    // orientation based on the connected faces.
    was_alone_ = false;
    orientation::Setup();
  } else {
    // We are not alone and was not alone the previous loop iteration. Check
    // what our neighbors think our orientation should be.
    FOREACH_FACE(face) {
      if (!didValueOnFaceChange(face)) {
        // No change on this face, just continue looking.
        continue;
      }

      // We got a new value on this face. Record it.
      byte face_value = getLastValueReceivedOnFace(face);

      if (face_value == 0) {
        // We use 0 to mean "no value set", so we ignore it.
        continue;
      }

      // We got some data. Set our orientation based on the real remote face
      // value (we always transmit relative face + 1).
      orientation::Setup(face_value - 1, face);

      // No reason to continue checking other faces. If they change, we will get
      // to them in other loop iterations.
      break;
    }
  }

  if (buttonSingleClicked()) {
    // Reset orientation to what we think is the best one. This will propagate
    // to all other Blinks.
    orientation::Setup();
  }

  // Make sure we clear precioulsy set faces.
  setColor(OFF);

  // Set the color of the abolute local face that maps to the global global face
  // 0 to blue.
  setColorOnFace(BLUE, orientation::AbsoluteLocalFace(0));

  // Report our relative face to all our neighbors. Again, we use relative face
  // + 1.
  FOREACH_FACE(face) {
    setValueSentOnFace(orientation::RelativeLocalFace(face) + 1, face);
  }
}
