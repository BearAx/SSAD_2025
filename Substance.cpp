#include <bits/stdc++.h>

using namespace std;

// Animal is the base class for all creatures.
class Animal
{
protected:
    const string name;
    int daysLived;

public:
    Animal(const string &n, int d) : name(n), daysLived(d) {}
    virtual ~Animal() {}
    const string &getName() const { return name; }
    int getDaysLived() const { return daysLived; }
    void setDaysLived(int d) { daysLived = d; }
    void incrementDay() { daysLived++; }

    // Each animal type must return a code like "M" for Mouse or "BM" for BetterMouse, plus a descriptive name like "Mouse" or "BetterMouse".
    virtual string getTypeCode() const = 0;
    virtual string getTypeName() const = 0;

    // sayName prints the animal’s name and how many days lived.
    virtual void sayName() const
    {
        cout << "My name is " << name << ", days lived: " << daysLived << "\n";
    }

    // attack is called when this animal tries to attack another.
    virtual void attack(Animal &other) = 0;

    // Identify the animal’s state: normal, better, or monster.
    virtual bool isNormal() const = 0;
    virtual bool isBetter() const = 0;
    virtual bool isMonster() const = 0;
};

// Fish, Bird, and Mouse are normal animals.
class Fish : public Animal
{
public:
    Fish(const string &n, int d) : Animal(n,d) {}
    Fish(const Fish &o) : Animal(o.name, o.daysLived) {}

    string getTypeCode() const override { return "F"; }
    string getTypeName() const override { return "Fish"; }

    void attack(Animal &other) override
    {
        cout << getTypeName() << " is attacking\n";
    }

    bool isNormal() const override { return true; }
    bool isBetter() const override { return false; }
    bool isMonster() const override { return false; }
};

class Bird : public Animal
{
public:
    Bird(const string &n, int d) : Animal(n,d) {}
    Bird(const Bird &other) : Animal(other.name, other.daysLived) {}

    string getTypeCode() const override { return "B"; }
    string getTypeName() const override { return "Bird"; }

    void attack(Animal &other) override
    {
        cout << getTypeName() << " is attacking\n";
    }

    bool isNormal() const override { return true; }
    bool isBetter() const override { return false; }
    bool isMonster() const override { return false; }
};

class Mouse : public Animal
{
public:
    Mouse(const string &n, int d) : Animal(n,d) {}
    Mouse(const Mouse &other) : Animal(other.name, other.daysLived) {}

    string getTypeCode() const override { return "M"; }
    string getTypeName() const override { return "Mouse"; }

    void attack(Animal &other) override
    {
        cout << getTypeName() << " is attacking\n";
    }

    bool isNormal() const override { return true; }
    bool isBetter() const override { return false; }
    bool isMonster() const override { return false; }
};

// BetterFish, BetterBird, and BetterMouse happen when substance is applied once to a normal.
class BetterFish : public Fish
{
public:
    // We halve the days to show they are "reborn" in a better form.
    BetterFish(const Fish &f) : Fish(f)
    {
        setDaysLived((f.getDaysLived() + 1) / 2);
    }
    BetterFish(const string &n, int d) : Fish(n,d) {}

    string getTypeCode() const override { return "BF"; }
    string getTypeName() const override { return "BetterFish"; }

    bool isNormal() const override { return false; }
    bool isBetter() const override { return true; }
    bool isMonster() const override { return false; }

    void attack(Animal &other) override
    {
        cout << getTypeName() << " is attacking\n";
    }
};

class BetterBird : public Bird
{
public:
    BetterBird(const Bird &b) : Bird(b)
    {
        setDaysLived((b.getDaysLived() + 1) / 2);
    }
    BetterBird(const string &n, int d) : Bird(n,d) {}

    string getTypeCode() const override { return "BB"; }
    string getTypeName() const override { return "BetterBird"; }

    bool isNormal() const override { return false; }
    bool isBetter() const override { return true; }
    bool isMonster() const override { return false; }

    void attack(Animal &other) override
    {
        cout << getTypeName() << " is attacking\n";
    }
};

class BetterMouse : public Mouse {
public:
    BetterMouse(const Mouse &m) : Mouse(m)
    {
        setDaysLived((m.getDaysLived() + 1) / 2);
    }
    BetterMouse(const string &n, int d) : Mouse(n,d) {}

    string getTypeCode() const override { return "BM"; }
    string getTypeName() const override { return "BetterMouse"; }

    bool isNormal() const override { return false; }
    bool isBetter() const override { return true; }
    bool isMonster() const override { return false; }

    void attack(Animal &other) override
    {
        cout << getTypeName() << " is attacking\n";
    }
};

// Monster is created if you apply substance again to an already better animal.
class Monster : public Animal
{
public:
    Monster(const string &n) : Animal(n, 1) {}
    Monster(const Animal &a) : Animal(a.getName(), 1) {}

    string getTypeCode() const override { return "MONSTER"; }
    string getTypeName() const override { return "Monster"; }

    void attack(Animal &other) override
    {
        cout << getTypeName() << " is attacking\n";
    }

    bool isNormal() const override { return false; }
    bool isBetter() const override { return false; }
    bool isMonster() const override { return true; }
};

// SortAnimals organizes them by days lived ascending, if tied by name ascending.
struct SortAnimals
{
    bool operator()(Animal* firstAnimal, Animal* secondAnimal) const
    {
        if(firstAnimal->getDaysLived() != secondAnimal->getDaysLived()) return firstAnimal->getDaysLived() < secondAnimal->getDaysLived();
        return firstAnimal->getName() < secondAnimal->getName();
    }
};

// ContainerBase manages a list of animals.
template<typename T>
class ContainerBase {
protected:
    vector<Animal*> animals;

public:
    virtual ~ContainerBase()
    {
        for(auto *animal : animals) delete animal;
        animals.clear();
    }
    // add an animal, then stable-sort them.
    void add(Animal* animal)
    {
        animals.push_back(animal);
        update();
    }
    // get returns a pointer by index, or nullptr if out of range.
    Animal* get(int idx)
    {
        if(idx < 0 || idx >= (int)animals.size()) return nullptr;
        return animals[idx];
    }
    // removeByIndex erases from the vector and returns the pointer.
    Animal* removeByIndex(int idx)
    {
        if(idx < 0 || idx >= (int)animals.size()) return nullptr;
        Animal* r = animals[idx];
        animals.erase(animals.begin() + idx);
        update();
        return r;
    }
    int size() const { return animals.size(); }
    // update calls stable_sort to keep them ordered.
    void update()
    {
        stable_sort(animals.begin(), animals.end(), SortAnimals());
    }
    // incrementDays increments each animal's age, kills them if they exceed their day limit.
    void incrementDays()
    {
        vector<int> dead;
        for (int i = 0; i < (int)animals.size(); i++)
        {
            animals[i]->incrementDay();
            // normal/better > 10 => dead, monster > 1 => dead.
            if(animals[i]->isMonster() && animals[i]->getDaysLived() > 1)
            {
                cout << animals[i]->getName() << " has died of old days\n";
                dead.push_back(i);
            }
            else if(!animals[i]->isMonster() && animals[i]->getDaysLived() > 10)
            {
                cout << animals[i]->getName() << " has died of old days\n";
                dead.push_back(i);
            }
        }
        // remove from back to front so indices do not shift.
        for (int i = (int)dead.size()-1; i >= 0; i--)
        {
            int idx = dead[i];
            delete animals[idx];
            animals.erase(animals.begin() + idx);
        }
        update();
    }
    // clear means kill everything at once.
    void clear()
    {
        for(auto *animal : animals)
        {
            cout << animal->getName() << " has died of old days\n";
            delete animal;
        }
        animals.clear();
    }
};

// Cage and Aquarium have special template restrictions for animals.
template<typename T> class Cage : public ContainerBase<T> { };
template<> class Cage<Fish> : public ContainerBase<Fish> { public: Cage() = delete; };
template<> class Cage<BetterFish> : public ContainerBase<BetterFish> { public: Cage() = delete; };

template<typename T> class Aquarium : public ContainerBase<T> { };
template<> class Aquarium<Bird> : public ContainerBase<Bird> { public: Aquarium() = delete; };
template<> class Aquarium<BetterBird> : public ContainerBase<BetterBird> { public: Aquarium() = delete; };

// Freedom can hold any animal (including monsters).
template<typename A> class Freedom : public ContainerBase<A> { };

// Manager has all the containers. doPeriod increments each container in the correct order.
struct Manager
{
    Cage<Bird> cageBird;
    Cage<BetterBird> cageBetterBird;
    Cage<Mouse> cageMouse;
    Cage<BetterMouse> cageBetterMouse;
    Aquarium<Fish> aquariumFish;
    Aquarium<BetterFish> aquariumBetterFish;
    Aquarium<Mouse> aquariumMouse;
    Aquarium<BetterMouse> aquariumBetterMouse;
    Freedom<Animal> freedom;

    // doPeriod: age every container. If animals exceed day limit, they're removed.
    void doPeriod()
    {
        cageBird.incrementDays();
        cageBetterBird.incrementDays();
        cageMouse.incrementDays();
        cageBetterMouse.incrementDays();
        aquariumFish.incrementDays();
        aquariumBetterFish.incrementDays();
        aquariumMouse.incrementDays();
        aquariumBetterMouse.incrementDays();
        freedom.incrementDays();
    }

    // pickMeContainer chooses the correct container by name and type code.
    ContainerBase<Animal>* pickContainer(const string &cont, const string &typeCode)
    {
        if(cont=="Freedom") return &freedom;
        if(cont=="Cage")
        {
            if(typeCode=="B")  return reinterpret_cast<ContainerBase<Animal>*>(&cageBird);
            if(typeCode=="BB") return reinterpret_cast<ContainerBase<Animal>*>(&cageBetterBird);
            if(typeCode=="M")  return reinterpret_cast<ContainerBase<Animal>*>(&cageMouse);
            if(typeCode=="BM") return reinterpret_cast<ContainerBase<Animal>*>(&cageBetterMouse);
            return nullptr;
        }
        if(cont=="Aquarium")
        {
            if(typeCode=="F")  return reinterpret_cast<ContainerBase<Animal>*>(&aquariumFish);
            if(typeCode=="BF") return reinterpret_cast<ContainerBase<Animal>*>(&aquariumBetterFish);
            if(typeCode=="M")  return reinterpret_cast<ContainerBase<Animal>*>(&aquariumMouse);
            if(typeCode=="BM") return reinterpret_cast<ContainerBase<Animal>*>(&aquariumBetterMouse);
            return nullptr;
        }
        return nullptr;
    }

    // makeAnimal builds a normal or better one from code.
    Animal* makeAnimal(const string &type, const string &name, int d)
    {
        if(type=="M")  return new Mouse(name,d);
        if(type=="B")  return new Bird(name,d);
        if(type=="F")  return new Fish(name,d);
        if(type=="BM") return new BetterMouse(name,d);
        if(type=="BB") return new BetterBird(name,d);
        if(type=="BF") return new BetterFish(name,d);
        return nullptr;
    }

    // handleCreate: create an animal, print a greeting, put it in container or freedom.
    void handleCreate(const string &type, const string &name, const string &cont, int d)
    {
        Animal* animal = makeAnimal(type, name, d);

        if(!animal) return;

        animal->sayName();
        if(cont=="Freedom") freedom.add(animal);

        else
        {
            auto c = pickContainer(cont, type);
            if(c) c->add(animal); else delete animal;
        }
    }

    // handleApplySubstanceFreedom: can not apply substance if in Freedom.
    void handleApplySubstanceFreedom(int pos)
    {
        cout << "Substance cannot be applied in freedom\n";
    }

    // handleApplySubstance: normal->better, or better->monster.
    void handleApplySubstance(const string &cont, const string &type, int pos)
    {
        if(cont=="Freedom")
        {
            handleApplySubstanceFreedom(pos);
            return;
        }

        auto containerAnimal = pickContainer(cont, type);

        if(!containerAnimal || pos < 0 || pos >= containerAnimal->size())
        {
            cout << "Animal not found\n";
            return;
        }

        Animal* oldAnimal = containerAnimal->get(pos);

        if(!oldAnimal || oldAnimal->isMonster())
        {
            cout << "Animal not found\n";
            return;
        }

        containerAnimal->removeByIndex(pos);

        if(oldAnimal->isNormal())
        {
            Animal* better = nullptr;
            if(oldAnimal->getTypeCode()=="M") better = new BetterMouse(*dynamic_cast<Mouse*>(oldAnimal));
            else if(oldAnimal->getTypeCode()=="B") better = new BetterBird(*dynamic_cast<Bird*>(oldAnimal));
            else if(oldAnimal->getTypeCode()=="F") better = new BetterFish(*dynamic_cast<Fish*>(oldAnimal));

            delete oldAnimal;

            if(!better) return;
            auto newC = pickContainer(cont, better->getTypeCode());
            if(newC) newC->add(better); else delete better;
        }

        else if(oldAnimal->isBetter())
        {
            Monster* mon = new Monster(*oldAnimal);
            delete oldAnimal;
            // If a monster is created, kills all in container, then monster goes to freedom.
            containerAnimal->clear();
            freedom.add(mon);
        }
    }

    // handleRemoveSubstanceFreedom: can not remove substance in Freedom.
    void handleRemoveSubstanceFreedom(int pos)
    {
        cout << "Substance cannot be removed in freedom\n";
    }

    // handleRemoveSubstance: better->normal by doubling days.
    void handleRemoveSubstance(const string &cont, const string &type, int pos)
    {
        if(cont=="Freedom")
        {
            handleRemoveSubstanceFreedom(pos);
            return;
        }

        auto containerAnimal = pickContainer(cont, type);

        if(!containerAnimal || pos < 0 || pos >= containerAnimal->size())
        {
            cout << "Animal not found\n";
            return;
        }

        Animal* oldAnimal = containerAnimal->get(pos);

        if(!oldAnimal || !oldAnimal->isBetter())
        {
            cout << "Invalid substance removal\n";
            return;
        }

        containerAnimal->removeByIndex(pos);

        Animal* normal = nullptr;

        if(oldAnimal->getTypeCode()=="BM") normal = new Mouse(oldAnimal->getName(), oldAnimal->getDaysLived()*2);
        else if(oldAnimal->getTypeCode()=="BB") normal = new Bird(oldAnimal->getName(), oldAnimal->getDaysLived()*2);
        else if(oldAnimal->getTypeCode()=="BF") normal = new Fish(oldAnimal->getName(), oldAnimal->getDaysLived()*2);

        delete oldAnimal;

        if(!normal) return;
        auto newContainerAnimal = pickContainer(cont, normal->getTypeCode());
        if(newContainerAnimal) newContainerAnimal->add(normal); else delete normal;
    }

    // handleAttackFreedom: no attacking in Freedom.
    void handleAttackFreedom(int pos1, int pos2)
    {
        cout << "Animals cannot attack in Freedom\n";
    }

    // handleAttack: remove the target, effectively killing them.
    void handleAttack(const string &container, const string &type, int pos1, int pos2)
    {
        if(container=="Freedom")
        {
            handleAttackFreedom(pos1, pos2);
            return;
        }

        auto containerAnimal = pickContainer(container, type);

        if(!containerAnimal)
        {
            cout << "Animal not found\n";
            return;
        }

        if(pos1 < 0 || pos1 >= containerAnimal->size() || pos2 < 0 || pos2 >= containerAnimal->size() || pos1 == pos2)
        {
            cout << "Animal not found\n";
            return;
        }

        Animal* attacker = containerAnimal->get(pos1);
        Animal* target = containerAnimal->get(pos2);

        if(!attacker || !target)
        {
            cout << "Animal not found\n";
            return;
        }

        attacker->attack(*target);
        Animal* dead = containerAnimal->removeByIndex(pos2);
        delete dead;
    }

    // handleTalkFreedom: talk if the animal is in Freedom.
    void handleTalkFreedom(int pos)
    {
        if(pos < 0 || pos >= freedom.size())
        {
            cout << "Animal not found\n";
            return;
        }

        Animal* a = freedom.get(pos);
        if(a) a->sayName(); else cout << "Animal not found\n";
    }

    // handleTalk: calls the container's animal's sayName.
    void handleTalk(const string &container, const string &type, int pos)
    {
        if(container=="Freedom")
        {
            handleTalkFreedom(pos);
            return;
        }

        auto containerAnimal = pickContainer(container, type);

        if(!containerAnimal || pos < 0 || pos >= containerAnimal->size())
        {
            cout << "Animal not found\n";
            return;
        }

        Animal* animal = containerAnimal->get(pos);

        if(!animal)
        {
            cout << "Animal not found\n";
            return;
        }

        animal->sayName();
    }
};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int C;
    cin >> C;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Manager manager;

    while(C--)
    {
        string line;
        if(!getline(cin, line)) break;
        if(line.empty()) continue;

        // Split the input line into tokens.
        vector<string> tk;
        {
            stringstream ss(line);
            string w;
            while(ss >> w) tk.push_back(w);
        }

        if(tk.empty()) continue;

        // The first token determines the command.
        string cmd = tk[0];

        if(cmd=="CREATE")
        {
            // CREATE <TYPE> <NAME> IN <CONTAINER> <days>.
            if(tk.size()<6) continue;
            string type = tk[1], name = tk[2];
            string container = tk[4]; // token[3] is "IN".
            int d = stoi(tk[5]);
            manager.handleCreate(type, name, container, d);
        }
        else if(cmd=="APPLY_SUBSTANCE")
        {
            // If container == Freedom, we get [APPLY_SUBSTANCE, Freedom, pos].
            // Otherwise [APPLY_SUBSTANCE, container, type, pos].
            if(tk.size()==3 && tk[1]=="Freedom")
            {
                int pos = stoi(tk[2]);
                manager.handleApplySubstanceFreedom(pos);
            }
            else if(tk.size()>=4)
            {
                string container = tk[1], type = tk[2];
                int pos = stoi(tk[3]);
                manager.handleApplySubstance(container, type, pos);
            }
        }
        else if(cmd=="REMOVE_SUBSTANCE")
        {
            // If container == Freedom => [REMOVE_SUBSTANCE, Freedom, pos].
            // Otherwise => [REMOVE_SUBSTANCE, container, type, pos].
            if(tk.size()==3 && tk[1]=="Freedom")
            {
                int pos = stoi(tk[2]);
                manager.handleRemoveSubstanceFreedom(pos);
            }
            else if(tk.size()>=4)
            {
                string container = tk[1], type = tk[2];
                int pos = stoi(tk[3]);
                manager.handleRemoveSubstance(container, type, pos);
            }
        }
        else if(cmd=="ATTACK")
        {
            // If container == Freedom => [ATTACK, Freedom, pos1, pos2].
            // Otherwise => [ATTACK, container, type, pos1, pos2].
            if(tk.size()==4 && tk[1]=="Freedom")
            {
                int p1 = stoi(tk[2]), p2 = stoi(tk[3]);
                manager.handleAttackFreedom(p1, p2);
            }
            else if(tk.size()>=5)
            {
                string container = tk[1], type = tk[2];
                int p1 = stoi(tk[3]), p2 = stoi(tk[4]);
                manager.handleAttack(container, type, p1, p2);
            }
        }
        else if(cmd=="TALK")
        {
            // If container == Freedom => [TALK, Freedom, pos].
            // Otherwise => [TALK, container, type, pos].
            if(tk.size()==3 && tk[1]=="Freedom")
            {
                int pos = stoi(tk[2]);
                manager.handleTalkFreedom(pos);
            }
            else if(tk.size()>=4)
            {
                string container = tk[1], type = tk[2];
                int pos = stoi(tk[3]);
                manager.handleTalk(container, type, pos);
            }
        }
        else if(cmd=="PERIOD")
        {
            // PERIOD => age all containers.
            manager.doPeriod();
        }
    }

    return 0;
}
