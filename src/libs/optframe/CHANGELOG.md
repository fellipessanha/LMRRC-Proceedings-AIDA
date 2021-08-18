
# OptFrame Changelog

## OptFrame 4.0.3 - development (bugfix)

- explored gcc bug "a variable concept cannot be constrained", meaning that during concepts 
definition we cannot replace auto rules (class R) by constrained ones (XRepresentation R)
   * Same as: "https://github.com/boostorg/asio/issues/312" => "https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67658"
- Fixing other bug "error: result type is not a plain type-constraint" (when variable concepts are put on return types, e.g., "-> XSolution&")

## OptFrame 4.0.2 (bugfix)

- Adjusted tests from deprecated [gtestlib](https://github.com/google/googletest) to [catch2](https://github.com/catchorg/Catch2)
- Fixed MCT and examples to standard format (`src/`, `tests/`, `build/`, etc)


## OptFrame 4.0.1 (bugfix)

- Fixing remote reference to a stable [Scanner](https://github.com/optframe/scannerpp) version.
- Fixing remote reference to deprecated [gtestlib](https://github.com/google/googletest).

## OptFrame 4.0-alpha

- Complete refactor from OptFrame 3, using `concepts`.
