#define HORIZONTAL (0)  /* place ship horizontally */
#define VERTICAL (1)    /* place ship vertically */

#define MAX_SHIP_LENGTH (17) /* length of longest ship (width is always 1) */

#define NO_SHIP_NAME ('.') /* what to return when hitting no ship */

/*
 * Type for coordinates, and their maximum possible value.
 *
 * Include <stdint.h> before this header file
 * to get the definition of uint32_t
 * and its maximum value UINT32_MAX.
 */
typedef uint32_t coord;
#define COORD_MAX (UINT32_MAX)

/*
 * Non-opaque structs for passing around positions and ship placements.
 */
struct position {
    coord x;
    coord y;
};

struct ship {
    struct position topLeft;  /* coordinates of top left corner */
    int direction;            /* HORIZONTAL or VERTICAL */
    unsigned int length;      /* length of ship */
    char name;                /* name of ship */
};

/*
 * Create a playing field for holding ships.
 */
struct field *fieldCreate(void);

/*
 * Free all space associated with a field.
 */
void fieldDestroy(struct field *);

/*
 * Place a ship in a field with given placement and name.
 *
 * If placement.length is less than one or greater than MAX_SHIP_LENGTH, 
 * or if some part of the ship would have a coordinate greater than COORD_MAX, 
 * or if the ship's name is NO_SHIP_NAME,
 * the function returns without placing a ship.
 *
 * Placing a new ship that intersects any previously-placed ships
 * sinks the previous ships, removing them from the field.
 */
void fieldPlaceShip(struct field *f, struct ship s);

/*
 * Attack!
 *
 * Drop a shell at given position.
 *
 * Returns NO_SHIP_NAME if attack misses (does not intersect any ship).
 *
 * Otherwise returns name of ship hit. 
 *
 * Hitting a ship sinks it, removing it from the field.
 */
char fieldAttack(struct field *f, struct position p);

/*
 * Return number of ships in the field.
 */
size_t fieldCountShips(const struct field *f);
