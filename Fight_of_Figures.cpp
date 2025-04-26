#include <iostream>
#include <string>
#include <map>
#include <sstream>

using namespace std;

/*
 * Enum class representing the two movement styles:
 * - NORMAL: The figure moves with a step size of 1.
 * - ATTACKING: The figure moves with a step size of 2.
 */
enum class Style { NORMAL, ATTACKING };

/*
 * MovementStrategy Interface (Strategy Pattern)
 * -----------------------------------------------
 * This abstract base class defines an interface for movement strategies.
 * Its purpose is to allow figures to determine their step sizes based on
 * their current style.
 *
 * Derived classes override the getStep() method to return a specific step size.
 */
class MovementStrategy
{
public:
    // Returns the number of steps a figure moves.
    virtual int getStep() const = 0;
    virtual ~MovementStrategy() {}
};

/*
 * NormalMovement Class
 * --------------------
 * Implements the MovementStrategy interface for the NORMAL style.
 * In this strategy, the step size is defined as 1.
 */
class NormalMovement : public MovementStrategy
{
public:
    int getStep() const override { return 1; }
};

/*
 * AttackingMovement Class
 * -----------------------
 * Implements the MovementStrategy interface for the ATTACKING style.
 * In this strategy, the step size is defined as 2.
 */
class AttackingMovement : public MovementStrategy
{
public:
    int getStep() const override { return 2; }
};

/*
 * Figure Class (Prototype Pattern)
 * ------------------------------------------------------
 * This abstract base class represents any game figure (either a main figure
 * or a clone). It contains data members for the figure's current position,
 * status (alive or not), team affiliation, name, and movement style.
 *
 * The class declares a virtual clone() method to support the Prototype pattern.
 * Derived classes must implement clone() to create a copy of the main figure.
 *
 * Other functionalities include changing the movement style, getting the
 * appropriate movement strategy (via the Strategy pattern), and general getters/setters.
 */
class Figure
{
protected:
    int positionX, positionY;   // Coordinates on the board.
    bool alive;                 // True if the figure is active (alive).
    bool teamGreen;             // True if the figure belongs to the green team; false for red.
    string name;                // Figure's name (e.g., "GREEN", "RED", "GREENCLONE", "REDCLONE").
    Style style;                // Current movement style (NORMAL or ATTACKING).

public:
    // Constructor initializes position, team and name. Default style is NORMAL.
    Figure(int x, int y, bool teamGreen, const string &name)
        : positionX(x), positionY(y), alive(true), teamGreen(teamGreen), name(name), style(Style::NORMAL) {}
    virtual ~Figure() {}

    // Virtual clone method to be implemented in derived classes.
    // It allows the creation of a new object with the same properties.
    virtual Figure* clone() const = 0;

    // Indicates whether this figure can be cloned. Main figures can clone; clones cannot.
    virtual bool canClone() const = 0;

    // Toggles the movement style between NORMAL and ATTACKING.
    virtual void changeStyle()
    {
        style = (style == Style::NORMAL) ? Style::ATTACKING : Style::NORMAL;
    }

    // Returns the appropriate movement strategy object based on the current style.
    // Caller is responsible for deleting the returned pointer.
    virtual MovementStrategy* getMovementStrategy() const
    {
        if (style == Style::NORMAL)
            return new NormalMovement();
        else
            return new AttackingMovement();
    }

    // Getter for the X-coordinate.
    int getX() const { return positionX; }
    // Getter for the Y-coordinate.
    int getY() const { return positionY; }

    // Updates the figure's position.
    void setPosition(int newPosX, int newPosY)
    {
        positionX = newPosX;
        positionY = newPosY;
    }

    // Returns whether the figure is alive.
    bool isAlive() const { return alive; }
    // Marks the figure as dead.
    void kill() { alive = false; }

    // Returns true if the figure belongs to the green team.
    bool isGreenTeam() const { return teamGreen; }
    // Returns the figure's name.
    string getName() const { return name; }
    // Returns the current movement style.
    Style getStyle() const { return style; }
};

/*
 * CloneFigure Class
 * -----------------
 * Represents a clone of a figure. This class inherits from Figure but
 * overrides the clone functionality so that clones cannot be cloned.
 *
 * Clones are created by the MainFigure class; they appear with swapped coordinates.
 */
class CloneFigure : public Figure
{
public:
    // Constructor passes parameters to the base Figure class.
    CloneFigure(int positionX, int positionY, bool teamGreen, const string &name)
        : Figure(positionX, positionY, teamGreen, name) {}

    // Clones cannot clone themselves; always returns false.
    bool canClone() const override { return false; }
    // clone() is not applicable to clones, so it returns nullptr.
    Figure* clone() const override { return nullptr; }
};

/*
 * MainFigure Class
 * ----------------
 * Represents a primary figure that can perform a clone operation.
 *
 * Inherits from Figure and implements the clone() method using the Prototype pattern.
 * When cloning, it creates a new CloneFigure with swapped coordinates (the clone's
 * position is [Y, X]). The clone's name is determined based on the team.
 */
class MainFigure : public Figure
{
public:
    // Constructor passes parameters to the base Figure class.
    MainFigure(int positionX, int positionY, bool teamGreen, const string &name)
        : Figure(positionX, positionY, teamGreen, name) {}

    // Main figures can be cloned.
    bool canClone() const override { return true; }

    // Creates a clone of the main figure using the Prototype pattern.
    // The new clone is a CloneFigure with swapped positionX and positionY.
    Figure* clone() const override
    {
        string cloneName = teamGreen ? "GREENCLONE" : "REDCLONE";
        return new CloneFigure(positionY, positionX, teamGreen, cloneName);
    }
};

/*
 * Board Class
 * -----------
 * Represents the game board. This class manages:
 *   - The board size.
 *   - Storage of coins placed on specific cells.
 *
 * Coins are stored in a std::map where the key is a pair (positionX, positionY)
 * and the value is the coin's value.
 *
 * The class provides methods for:
 *   - Adding coins.
 *   - Checking if a coin exists at a given cell.
 *   - Collecting (removing) a coin from a cell.
 *   - Verifying that a cell's coordinates are within the bounds of the board.
 */
class Board
{
    int size;  // Size (N) of the board (board is N x N).
    map<pair<int, int>, int> coins; // Map storing coins with cell coordinates as key.
public:
    // Initializes the board with a given size.
    Board(int n) : size(n) {}

    // Adds a coin at the specified position with its value.
    void addCoin(int positionX, int positionY, int value)
    {
        coins[{positionX, positionY}] = value;
    }

    // Returns true if a coin exists at the specified position.
    bool hasCoin(int positionX, int positionY) const
    {
        return coins.count({positionX, positionY}) > 0;
    }

    // If a coin is present, returns its value and removes it from the board.
    int collectCoin(int positionX, int positionY)
    {
        if (hasCoin(positionX, positionY)) {
            int value = coins.at({positionX, positionY});
            coins.erase({positionX, positionY});
            return value;
        }
        return 0;
    }

    // Returns true if the specified coordinates are within the board boundaries.
    bool isWithinBounds(int positionX, int positionY) const
    {
        return (positionX >= 1 && positionX <= size
            && positionY >= 1 && positionY <= size);
    }
};

/*
 * Game Class (Facade Pattern)
 * ---------------------------
 * The Game class serves as a facade that brings together the board (and its coins)
 * and the figures (main figures and clones), managing the overall game state.
 *
 * This class performs the following responsibilities:
 *   - Initializes the game board and figures.
 *   - Processes game actions (e.g., movement, style change, cloning) by applying game rules.
 *   - Tracks the score for the green and red teams.
 *   - Provides methods to check cell occupancy and handle enemy interactions.
 *   - Generates the final game result.
 *
 * The class holds pointers to the two main figures (green and red) as well as their
 * clones (if created). Memory is managed within the class destructor.
 */
class Game
{
    Board board; // The game board holding coin placements and dimensions.
    Figure* greenFigure; // Pointer to the green team's main figure.
    Figure* redFigure;   // Pointer to the red team's main figure.
    Figure* greenClone;  // Pointer to the green team's clone (if any).
    Figure* redClone;    // Pointer to the red team's clone (if any).
    long long greenScore; // The accumulated score for the green team.
    long long redScore;   // The accumulated score for the red team.

public:
    // Constructor initializes the board and sets initial pointers and scores.
    Game(int boardSize)
        : board(boardSize), greenFigure(nullptr), redFigure(nullptr),
          greenClone(nullptr), redClone(nullptr), greenScore(0), redScore(0) {}

    // Destructor cleans up dynamically allocated figure objects.
    ~Game()
    {
        if (greenFigure)
            delete greenFigure;
        if (redFigure)
            delete redFigure;
        if (greenClone)
            delete greenClone;
        if (redClone)
            delete redClone;
    }

    // Returns a reference to the game board for adding coins or boundary checks.
    Board& getBoard() { return board; }
    // Getter for the green team's score.
    long long getGreenScore() const { return greenScore; }
    // Getter for the red team's score.
    long long getRedScore() const { return redScore; }

    /*
     * Initializes the main figures for both teams with provided coordinates.
     * 'greenPositionX/Y' for the green team and 'redPositionX/Y' for the red team.
     */
    void initFigures(int greenPositionX, int greenPositionY, int redPositionX, int redPositionY)
    {
        greenFigure = new MainFigure(greenPositionX, greenPositionY, true, "GREEN");
        redFigure = new MainFigure(redPositionX, redPositionY, false, "RED");
    }

    /*
     * Returns the pointer to a figure that matches the provided name.
     * The function checks among the main figures and clones for both teams.
     */
    Figure* getFigureByName(const string &name)
    {
        if (greenFigure
            && greenFigure->getName() == name) {
            return greenFigure;
        }
        if (redFigure
            && redFigure->getName() == name) {
            return redFigure;
        }
        if (greenClone
            && greenClone->getName() == name) {
            return greenClone;
        }
        if (redClone
            && redClone->getName() == name) {
            return redClone;
        }
        return nullptr;
    }

    /*
     * Checks if a cell at the given coordinates is occupied by any living figure.
     */
    bool isOccupied(int positionX, int positionY) const
    {
        if (greenFigure
            && greenFigure->isAlive()
            && greenFigure->getX() == positionX
            && greenFigure->getY() == positionY) {
            return true;
        }
        if (redFigure
            && redFigure->isAlive()
            && redFigure->getX() == positionX
            && redFigure->getY() == positionY) {
            return true;
        }
        if (greenClone
            && greenClone->isAlive()
            && greenClone->getX() == positionX
            && greenClone->getY() == positionY) {
            return true;
        }
        if (redClone
            && redClone->isAlive()
            && redClone->getX() == positionX
            && redClone->getY() == positionY) {
            return true;
        }
        return false;
    }

    /*
     * Checks if the cell is occupied by an allied figure relative to the provided figure.
     * This is used to prevent moving a figure into a space occupied by a teammate.
     */
    bool isAlliedOccupied(const Figure* figure, int positionX, int positionY) const
    {
        if (greenFigure
            && greenFigure->isAlive()
            && greenFigure->getX() == positionX
            && greenFigure->getY() == positionY
            && greenFigure != figure
            && greenFigure->isGreenTeam() == figure->isGreenTeam()) {
            return true;
        }
        if (redFigure
            && redFigure->isAlive()
            && redFigure->getX() == positionX
            && redFigure->getY() == positionY
            && redFigure != figure
            && redFigure->isGreenTeam() == figure->isGreenTeam()) {
            return true;
        }
        if (greenClone
            && greenClone->isAlive()
            && greenClone->getX() == positionX
            && greenClone->getY() == positionY
            && greenClone != figure
            && greenClone->isGreenTeam() == figure->isGreenTeam()) {
            return true;
        }
        if (redClone
            && redClone->isAlive()
            && redClone->getX() == positionX
            && redClone->getY() == positionY
            && redClone != figure
            && redClone->isGreenTeam() == figure->isGreenTeam()) {
            return true;
        }
        return false;
    }

    /*
     * Checks if an enemy figure is present at the given cell.
     * If found, returns the enemy figure's name; otherwise, returns an empty string.
     */
    string enemyAttack(int positionX, int positionY, bool teamGreen)
    {
        if (greenFigure
            && greenFigure->isAlive()
            && greenFigure->getX() == positionX
            && greenFigure->getY() == positionY
            && greenFigure->isGreenTeam() != teamGreen) {
            return greenFigure->getName();
        }
        if (redFigure
            && redFigure->isAlive()
            && redFigure->getX() == positionX
            && redFigure->getY() == positionY
            && redFigure->isGreenTeam() != teamGreen) {
            return redFigure->getName();
        }
        if (greenClone
            && greenClone->isAlive()
            && greenClone->getX() == positionX
            && greenClone->getY() == positionY
            && greenClone->isGreenTeam() != teamGreen) {
            return greenClone->getName();
        }
        if (redClone
            && redClone->isAlive()
            && redClone->getX() == positionX
            && redClone->getY() == positionY
            && redClone->isGreenTeam() != teamGreen) {
            return redClone->getName();
        }
        return "";
    }

    /*
     * Processes an action based on the figure's name and a command string.
     *
     * Recognized commands:
     *   - "STYLE": Toggles the figure's movement style.
     *   - "COPY": Clones the figure (if allowed) into a position defined by swapping coordinates.
     *   - "UP", "DOWN", "LEFT", "RIGHT": Moves the figure in the specified direction.
     *
     * The method checks all game rules
     * and returns a descriptive string indicating the outcome of the action.
     */
    string processAction(const string &figureName, const string &action)
    {
        Figure* figure = getFigureByName(figureName);
        if (!figure || !figure->isAlive())
            return "INVALID ACTION";

        // Process the STYLE command:
        if (action == "STYLE") {
            figure->changeStyle();
            string newStyle = (figure->getStyle() == Style::NORMAL) ? "NORMAL" : "ATTACKING";
            return figure->getName() + " CHANGED STYLE TO " + newStyle;
        } else if (action == "COPY") { // Process COPY action
            // Check if the figure is allowed to clone.
            if (!figure->canClone())
                return "INVALID ACTION";
            // Cloning is not allowed if the figure is positioned on the diagonal (x == y).
            if (figure->getX() == figure->getY())
                return "INVALID ACTION";
            // Determine target cell by swapping coordinates.
            int targetX = figure->getY();
            int targetY = figure->getX();
            // Ensure that the target cell is not already occupied by any figure or a coin.
            if (isOccupied(targetX, targetY)
                || board.hasCoin(targetX, targetY)) {
                return "INVALID ACTION";
            }
            // Create a clone using the Prototype pattern.
            Figure* newClone = figure->clone();
            if (newClone->isGreenTeam())
                greenClone = newClone;
            else
                redClone = newClone;
            return (figure->isGreenTeam() ? "GREEN" : "RED") + string(" CLONED TO ")
                   + to_string(targetX) + " " + to_string(targetY);
        } else { // Process movement commands:
            int directionX = 0, directionY = 0;
            if (action == "UP") directionX = -1;
            else if (action == "DOWN") directionX = 1;
            else if (action == "LEFT") directionY = -1;
            else if (action == "RIGHT") directionY = 1;

            // Retrieve the movement strategy based on the figure's current style.
            MovementStrategy* movementStrategy = figure->getMovementStrategy();
            int step = movementStrategy->getStep();
            delete movementStrategy;  // Clean up the temporary strategy instance.

            // Compute the target cell coordinates based on the direction and step size.
            int targetX = figure->getX() + directionX * step;
            int targetY = figure->getY() + directionY * step;

            // Verify that the new position is within board boundaries.
            if (!board.isWithinBounds(targetX, targetY))
                return "INVALID ACTION";

            // Check that no allied figure occupies the target cell.
            if (isAlliedOccupied(figure, targetX, targetY))
                return "INVALID ACTION";

            // Check if there is an enemy figure at the target location.
            string enemy = enemyAttack(targetX, targetY, figure->isGreenTeam());
            bool enemyKilled = false;
            if (!enemy.empty()) {
                enemyKilled = true;
                // Retrieve the enemy figure and mark it as killed.
                Figure* enemyFigure = getFigureByName(enemy);
                if (enemyFigure)
                    enemyFigure->kill();
            }

            // Check if a coin exists at the target cell and collect it if present.
            bool coinsCollected = false;
            int coinsValue = 0;
            if (board.hasCoin(targetX, targetY)) {
                coinsCollected = true;
                coinsValue = board.collectCoin(targetX, targetY);
                if (figure->isGreenTeam())
                    greenScore += coinsValue;
                else
                    redScore += coinsValue;
            }

            // Update the figure's position after a successful move.
            figure->setPosition(targetX, targetY);

            // Build the result message detailing the move.
            ostringstream resultStream;
            resultStream << figure->getName() << " MOVED TO " << targetX << " " << targetY;
            if (enemyKilled)
                resultStream << " AND KILLED " << enemy;
            else if (coinsCollected)
                resultStream << " AND COLLECTED " << coinsValue;
            return resultStream.str();
        }
    }

    /*
     * Constructs and returns the final game result as a string.
     * It compares the scores of the green and red teams and formats the output accordingly.
     */
    string finalResult()
    {
        ostringstream resultStream;
        if (greenScore == redScore)
            resultStream << "TIE. SCORE " << greenScore << " " << redScore;
        else if (greenScore > redScore)
            resultStream << "GREEN TEAM WINS. SCORE " << greenScore << " " << redScore;
        else
            resultStream << "RED TEAM WINS. SCORE " << greenScore << " " << redScore;
        return resultStream.str();
    }
};

/*
 * Main function:
 * --------------
 * Entry point for the program.
 * 1. Reads input: board size, coordinates for main figures, coin placements, and actions.
 * 2. Initializes the Game object and sets up the board and main figures.
 * 3. Processes each action using the Game::processAction method.
 * 4. Outputs the result of each action followed by the final game result.
 */
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;
    Game game(N);

    // Read coordinates for the main figures.
    int greenPositionX, greenPositionY, redPositionX, redPositionY;
    cin >> greenPositionX >> greenPositionY;
    cin >> redPositionX >> redPositionY;
    game.initFigures(greenPositionX, greenPositionY, redPositionX, redPositionY);

    // Read coins and add them to the board.
    int M;
    cin >> M;
    for (int i = 0; i < M; i++) {
        int coinsPositionX, coinsPositionY, value;
        cin >> coinsPositionX >> coinsPositionY >> value;
        game.getBoard().addCoin(coinsPositionX, coinsPositionY, value);
    }

    // Read and process each game action.
    int P;
    cin >> P;
    string figureName, action;
    // Actions are processed; it is assumed the turns alternate appropriately.
    for (int i = 0; i < P; i++) {
        cin >> figureName >> action;
        cout << game.processAction(figureName, action) << "\n";
    }

    // Output the final game result.
    cout << game.finalResult() << "\n";

    return 0;
}
