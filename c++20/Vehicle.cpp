#include<cstring>
#include<iostream>
#include<cinttypes>

using namespace std;

class Vehicle
{
private:
    char * name;
    uint32_t NumofWheels;
public:
    Vehicle()=default;
    Vehicle(const char * name, uint32_t NumofWheels);
    Vehicle(const Vehicle & other);
    Vehicle(Vehicle&& other);
    Vehicle& operator=(const Vehicle & other);
    Vehicle& operator=(Vehicle&& other);
    ~Vehicle();
    char * getName() const
    {
        return name;
    }
};
    Vehicle::Vehicle(const char * name, uint32_t NumofWheels)
    //:name(nullptr)
    {
        delete [] this->name;
        this->name=new char[strlen(name) + 1];
        strcpy(this->name,name);
        this->NumofWheels =NumofWheels;
    }
    Vehicle::Vehicle(const Vehicle & other)
    //:name(nullptr)
    {
        delete [] name;
        this->name=new char[strlen(other.name) + 1];
        strcpy(this->name,other.name);
        this->NumofWheels =other.NumofWheels;
    }
    Vehicle::Vehicle(Vehicle&& other)
    {
        name=other.name;
        other.name=nullptr;
        NumofWheels=other.NumofWheels; 
    }
    Vehicle& Vehicle::operator=(const Vehicle & other)
    {
        if(this!=&other)
        {
            delete [] name;
            this->name=new char[strlen(other.name) + 1];
            strcpy(this->name,other.name);
            this->NumofWheels =other.NumofWheels;
        }
        return *this;
    }
    Vehicle& Vehicle::operator=(Vehicle&& other)
    {
        this->name=other.name;
        other.name=nullptr;
        this->NumofWheels=other.NumofWheels;
        return *this;
    }
    Vehicle::~Vehicle()
    {
        delete[] name;
    }


   int main()
   {
        Vehicle car("xiaomi",4);
        Vehicle car2(car);
        //std::move(car);
        cout<<car.getName()<<endl;
        cout<<car2.getName()<<endl;

        return 0;
   }