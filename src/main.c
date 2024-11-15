#include "game.h"
#include "khg_phy/body.h"
#include "khg_phy/constraints/constraint.h"
#include "khg_phy/constraints/hinge_constraint.h"
#include "khg_phy/core/phy_vector.h"
#include "khg_phy/shape.h"
#include "khg_phy/space.h"
#include "networking/client.h"
#include "networking/server.h"
#include "networking/test.h"
#include <stdio.h>
#include <string.h>

const int main(int argc, char *argv[]) {
  phy_space *space = phy_space_new();
  phy_rigid_body_initializer body_init = phy_rigid_body_initializer_default;
  body_init.type = PHY_RIGID_BODY_TYPE_STATIC;
  body_init.position = phy_vector2_new(50.0, 15.0);
  phy_rigid_body *body0 = phy_rigid_body_new(body_init);

  phy_shape *body0_shape = phy_rect_shape_new(4.0, 2.0, phy_vector2_zero);
  phy_rigid_body_add_shape(body0, body0_shape);

  phy_space_add_rigidbody(space, body0);


  body_init.type = PHY_RIGID_BODY_TYPE_DYNAMIC;
  body_init.position = phy_vector2_new(50.0 + 4.0, 15.0);
  phy_rigid_body *body1 = phy_rigid_body_new(body_init);

  phy_shape *body1_shape = phy_rect_shape_new(4.0, 2.0, phy_vector2_zero);
  phy_rigid_body_add_shape(body1, body1_shape);

  phy_space_add_rigidbody(space, body1);


  body_init.position = phy_vector2_new(50.0 - 4.0, 15.0);
  phy_rigid_body *body2 = phy_rigid_body_new(body_init);

  phy_shape *body2_shape = phy_rect_shape_new(4.0, 2.0, phy_vector2_zero);
  phy_rigid_body_add_shape(body2, body2_shape);

  phy_space_add_rigidbody(space, body2);


  phy_hinge_constraint_initializer cons_init = phy_hinge_constraint_initializer_default;
  cons_init.a = body0;
  cons_init.b = body1;
  cons_init.anchor = phy_vector2_new(50.0 + 2.0, 15.0);
  cons_init.enable_limits = true;
  cons_init.lower_limit = -M_PI * 0.5;
  cons_init.upper_limit = M_PI * 0.5;
  phy_constraint *hinge_cons0 = phy_hinge_constraint_new(cons_init);
  phy_space_add_constraint(space, hinge_cons0);

  cons_init.a = body0;
  cons_init.b = body2;
  cons_init.anchor = phy_vector2_new(50.0 - 2.0, 15.0);
  cons_init.lower_limit = 0.0;
  cons_init.upper_limit = M_PI * 0.25;
  phy_constraint *hinge_cons1 = phy_hinge_constraint_new(cons_init);
  phy_space_add_constraint(space, hinge_cons1);

  // Ignore collision of bodies connected with hinge constraint
  hinge_cons0->ignore_collision = true;
  hinge_cons1->ignore_collision = true;

  printf("%f, %f\n", body1->position.x, body1->position.y);
  phy_space_step(space, 0.016f);
  printf("%f, %f\n", body1->position.x, body1->position.y);
  phy_space_step(space, 0.016f);
  printf("%f, %f\n", body1->position.x, body1->position.y);
  phy_space_step(space, 0.016f);

  phy_space_remove_constraint(space, hinge_cons0);
  phy_space_remove_constraint(space, hinge_cons1);
  phy_constraint_free(hinge_cons0);
  phy_constraint_free(hinge_cons1);
  phy_rigid_body_remove_shape(body0, body0_shape);
  phy_rigid_body_remove_shape(body1, body1_shape);
  phy_rigid_body_remove_shape(body2, body2_shape);
  phy_shape_free(body0_shape);
  phy_shape_free(body1_shape);
  phy_shape_free(body2_shape);
  phy_space_remove_rigidbody(space, body0);
  phy_space_remove_rigidbody(space, body1);
  phy_space_remove_rigidbody(space, body2);
  phy_rigid_body_free(body0);
  phy_rigid_body_free(body1);
  phy_rigid_body_free(body2);
  phy_space_free(space);
  return 0;
  if (argc > 1) {
    if (strcmp(argv[1], "hoster") == 0) {
      hoster_run();
    }
    else if (strcmp(argv[1], "joiner") == 0) {
      joiner_run();
    }
  }
  else {
    return game_run();
    return main_dbm();
    server_run();
  }
  return 0;
}

