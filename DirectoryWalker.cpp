#include <bits/stdc++.h>

using namespace std;

// Flyweight
// FileProperties: stores metadata for files like extension, read-only flag, owner, and group.
// We treat these properties as immutable so they can be shared safely.
struct FileProperties
{
    string extension; // File extension (e.g., "txt", "cpp")
    bool readOnly; // Whether the file is read-only
    string owner;
    string group; // File owner and group

    // Constructor initializes all properties
    FileProperties(string extension, bool readOnly, string owner, string group)
        : extension(move(extension)), readOnly(readOnly),
            owner(move(owner)), group(move(group)) {}
};

// FilePropertiesFactory: implements the Flyweight pattern
// It caches FileProperties objects so that identical property sets share one instance.
class FilePropertiesFactory
{
    // Key struct uniquely identifies a set of properties
    struct Key
    {
        string extension;
        bool readOnly;
        string owner;
        string group;

        bool operator == (const Key& k) const noexcept
        {
            return extension == k.extension
                    && readOnly == k.readOnly
                    && owner == k.owner
                    && group==k.group;
        }
    };
    // Hash functor for Key, so we can use it in an unordered_map
    struct KeyHash
    {
        size_t operator()(const Key& k) const noexcept
        {
            size_t hFunctor = hash<string>{}(k.extension);
            hFunctor = (hFunctor * 31) + hash<bool>{}(k.readOnly);
            hFunctor = (hFunctor * 31) + hash<string>{}(k.owner);
            hFunctor = (hFunctor * 31) + hash<string>{}(k.group);
            return hFunctor;
        }
    };

    // Static cache mapping each Key to a shared FileProperties instance
    static unordered_map<Key, shared_ptr<FileProperties>, KeyHash> cache;

public:
    // Returns a shared pointer to a FileProperties object matching the inputs.
    // If it doesn't exist yet, create and cache it.
    static shared_ptr<FileProperties> get(const string& extension, bool readOnly,
                                          const string& owner, const string& group)
    {
        Key key{extension, readOnly, owner, group};
        auto it = cache.find(key);
        if(it!=cache.end())
            return it->second;
        // Not found, create a new FileProperties and store it
        auto fileProperties = make_shared<FileProperties>(extension, readOnly, owner,group);
        cache.emplace(move(key),fileProperties);
        return fileProperties;
    }
};

// Define the static cache instance
unordered_map<FilePropertiesFactory::Key,
              shared_ptr<FileProperties>,
              FilePropertiesFactory::KeyHash>
    FilePropertiesFactory::cache;

// Visitor interface declares operations for File and Directory nodes.
class File;
class Directory;

struct Visitor
{
    virtual void visit(File& file ) = 0; // Handle File nodes
    virtual void visit(Directory& directory) = 0; // Handle Directory nodes
    virtual ~Visitor() = default;
};

// Node: common base for File and Directory
class Node : public enable_shared_from_this<Node>
{
protected:
    string name; // Name of the file or directory

public:
    explicit Node(string name):name(move(name)){}

    // Accept a visitor
    virtual void accept(Visitor& visitor) = 0;
    // Check if this node is a directory
    virtual bool isDirectory() const =0;
    // Get the node's name
    const string& getName() const { return name; }
    virtual ~Node() = default;
};

// File: terminal node that stores its size and a pointer to shared FileProperties
class File : public Node
{
    double sizeKB; // File size in KB
    shared_ptr<FileProperties> props; // Shared file metadata

public:
    File(string name,double size,shared_ptr<FileProperties> properties)
        : Node(move(name)), sizeKB(size), props(move(properties)) {}

    double getSize() const { return sizeKB; }
    shared_ptr<FileProperties> getProps() const { return props; }

    // Accept a visitor: let it process this File
    void accept(Visitor& visitor) override { visitor.visit(*this); }
    bool isDirectory() const override { return false; }
};

// Iterator interface for traversing Nodes
class Iterator
{
public:
    virtual bool hasNext() = 0;
    virtual shared_ptr<Node> next() = 0;
    virtual ~Iterator() = default;
};

class Directory; // Forward declaration

// DirectoryIterator: depth-first traversal over a directory tree
class DirectoryIterator : public Iterator
{
    // Stack holds pairs of (directory ptr, next child index)
    vector<pair<shared_ptr<Directory>,size_t>> stk;

public:
    explicit DirectoryIterator(shared_ptr<Directory> root);
    bool hasNext() override;
    shared_ptr<Node> next() override;
};

// Directory: node that contains children (files or subdirectories)
class Directory : public Node
{
    vector<shared_ptr<Node>> children;  // List of child nodes

public:
    explicit Directory(string name) : Node(move(name)) {}

    // Add a child node (file or directory)
    void addChild(shared_ptr<Node> name) { children.emplace_back(move(name)); }
    const vector<shared_ptr<Node>>& getChildren() const { return children; }

    // Factory method to get an iterator for this directory
    unique_ptr<Iterator> createIterator()
    {
        // Use shared_from_this to get a shared_ptr<Directory> for this
        return make_unique<DirectoryIterator>(
            static_pointer_cast<Directory>(shared_from_this()));
    }

    // Accept a visitor: first visit this directory, then its children
    void accept(Visitor& visitor) override
    {
        visitor.visit(*this);
        for (auto& children : children) children->accept(visitor);
    }
    bool isDirectory() const override { return true; }
};

// SizeVisitor: implements Visitor to sum up file sizes in a subtree
class SizeVisitor : public Visitor
{
    double total = 0.0;  // Running total of sizes

public:
    // When visiting a File, add its size
    void visit(File& file) override { total += file.getSize(); }
    // Directories don't contribute directly
    void visit(Directory&) override {}
    double getTotal() const { return total; }
};

// Formatting helpers
// Convert a size in KB to a string, with no decimals if integer, else one decimal
string sizeStr(double size)
{
    stringstream stringS;
    if(fabs(size - round(size)) < 1e-6)
        stringS << (long long)round(size);
    else
        stringS << fixed << setprecision(1) << size;
    return stringS.str() + "KB";
}

// Recursively print the directory tree in a Linux-style format
void printTree(const shared_ptr<Node>& node,
               const string& prefix, bool last)
{
    if (node->isDirectory()) {
        if (prefix.empty())
            cout << ".\n"; // Root

        auto* dir = static_cast<Directory*>(node.get());
        const auto& children = dir->getChildren();
        for (size_t i = 0; i < children.size(); ++i)
        {
            bool isLast = (i == children.size() - 1);
            cout << prefix << (isLast ? "└── " : "├── ")
                 << children[i]->getName();
            if (!children[i]->isDirectory()) {
                auto* f = static_cast<File*>(children[i].get());
                cout << " (" << sizeStr(f->getSize()) << ")";
            }
            cout << "\n";
            if (children[i]->isDirectory()) {
                // Indent for children
                printTree(children[i], prefix + (isLast ? "    " : "│   "), isLast);
            }
        }
    }
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    size_t N;
    if (!(cin >> N))
        return 0;

    // Map from directory ID to Directory pointer, root is ID 0
    unordered_map<long long, shared_ptr<Directory>> dirs;
    auto root = make_shared<Directory>(""); // Root has empty name
    dirs[0] = root;

    string operation;

    for (size_t i = 0; i < N; ++i)
    {
        cin >> operation;
        if (operation == "DIR") {
            long long id;
            string name;
            string maybeParent;

            cin >> id;

            getline(cin, maybeParent); // Read rest of line

            stringstream stringS(maybeParent);
            long long parentId = 0;

            if (!(stringS >> name)) continue; // Skip if no name
                string remain;
            if (stringS >> remain) { // If parent ID was provided
                parentId = stoll(name);
                name = remain;
            }

            auto dir = make_shared<Directory>(name);
            dirs[id] = dir;
            dirs[parentId]->addChild(dir);
        } else if (operation == "FILE") {
            long long parentId;
            string readOnlyString, owner, group, nameExtension;
            double size;

            cin >> parentId >> readOnlyString >> owner >> group >> size >> nameExtension;

            bool readOnly = (readOnlyString == "T");
            auto position = nameExtension.find_last_of('.');
            string name = nameExtension.substr(0, position);
            string extension = (position == string::npos ? "" : nameExtension.substr(position + 1));
            // Get shared properties object

            auto props = FilePropertiesFactory::get(extension, readOnly, owner, group);
            auto file = make_shared<File>(nameExtension, size, props);
            dirs[parentId]->addChild(file);
        }
    }

    // Use visitor to compute total size
    SizeVisitor sizeVisitor;
    root->accept(sizeVisitor);
    cout << "total: " << sizeStr(sizeVisitor.getTotal()) << '\n';

    // Print the tree structure
    printTree(root, "", true);
    return 0;
}
