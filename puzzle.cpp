#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <functional>
#include <queue>


using namespace std;


class Car;
class Passenger;
int CalculateScore(Passenger* p, Car* c, int Time);


struct Place {
    int x, y;
};


class Car {
    public:
    Place currPos;
    bool available;
    int id;

    std::vector<int> passengersMoved;

    Car() {}
    Car(int x, int y, int _id) {
        currPos.x = 0;
        currPos.y = 0;
        available = true;
        id = _id;
    }
};

class Passenger {
    public:
    Place start, finish;
    int pathLength;
    int earliestStart, latestFinish, id;
    bool inTransist = false;

    std::vector<Car*>* carList;
    Car* currCar;
    
    ~Passenger() {
        currCar = nullptr;
        carList = nullptr;
    }

    Passenger() {}
    
    Passenger(int _startRow, int _startCol, int _finishRow, int _finishCol,
         int _earliestStart, int _latestFinish, int _id) : earliestStart(_earliestStart), latestFinish(_latestFinish), id(_id) {
             start.x = _startRow;
             start.y = _startCol;
             finish.x = _finishRow;
             finish.y = _finishCol;
             inTransist = false;
             currCar = nullptr;

            pathLength = abs(finish.x - start.x) + abs(finish.y - start.y);


    }

    int FindCar(int T) {

        int min = 1000000;
        int minIndex = -1;

        priority_queue<int> cars;
        int carIndex;
        // for (auto c: *carList) {
        for (int i = 0; i < carList->size(); i++) {
            if (carList->at(i)->available) {
                carIndex = CalculateScore(this, carList->at(i), T);
                
                if (carIndex < min) {
                    min = carIndex;
                    minIndex = i;
                }
                    
                // if (carIndex  >= 0)
                //     cars.push(carIndex);
            }
        }

        // if (!cars.empty()) {
        //     return cars.top();
        // } 
        // return -1;
        if (minIndex >= 0)
            currCar = carList->at(minIndex);
        return minIndex;
    }



    bool Move() {
        if (--pathLength == 0){
            
            currCar->currPos = finish;
            currCar->passengersMoved.push_back(id);
            return true;
        }
        return false;
    }
};


int CalculateScore(Passenger* p, Car* c, int Time) {

    int c_p_distance = abs(p->start.x - c->currPos.x) + abs(p->start.y - c->currPos.y);
    int pathLength = abs(p->finish.x - p->start.x) + abs(p->finish.y - p->start.y);

    int TimeToArrive = Time + c_p_distance;
    if (p->earliestStart > (Time + c_p_distance)) {
        TimeToArrive = p->earliestStart;
    }
    TimeToArrive += pathLength;
    // TimeToArrive = TimeToArrive - pathLength;

    if (TimeToArrive > p->latestFinish) {

        return -1;
    }
    return abs(TimeToArrive);

}

std::vector<Passenger *> readInFile(ifstream& inFile, int rows, int cols) {

    std::string line = "";
    std::vector<Passenger *> passengers(rows);
    int passengerId = 0;
    int i = 0;
    int constArgs[6];
    
    while (i < rows) {
        inFile >> constArgs[0];
        inFile >> constArgs[1];
        inFile >> constArgs[2];
        inFile >> constArgs[3];
        inFile >> constArgs[4];
        inFile >> constArgs[5];

        passengers[i] = new Passenger(constArgs[0], constArgs[1], constArgs[2], constArgs[3], constArgs[4], constArgs[5], passengerId++);
        // std::cout   << passengers[i]->id << " "<< passengers[i]->start.x << " " << passengers[i]->start.y << " " << passengers[i]->finish.x << " "
        //             << passengers[i]->finish.y << " " << passengers[i]->earliestStart << " " << passengers[i]->latestFinish;

        // std::cout << "\n";
        i++;
    }

    return passengers;
}


int main(int argc, char** argv) {
    std::ifstream inFile;
    std::ofstream outFile;

    int rows, cols, numVehicles, numRides, bonus, numSteps;

    if (argc > 1) {
        inFile.open(argv[1]);
        if (!inFile) {
            std::cout << "Cannot read file: " << argv[1] << "\n";
            return 1;
        }
        std::cout << "Reading from: " << argv[1] << "\n";
    } else {
        std::cout << "Please provide an inFile file.\n";
        return 1;
    }
    
    if (argc > 2) {
        outFile.open(argv[2]);
        if (!outFile) {
            std::cout << "Cannot open file to write: " << argv[2] << "\n";
            return 1;
        } else {
            std::cout << "Using file to write outFile: " << argv[2] << "\n";
        }
    }


    inFile >> rows;
    inFile >> cols;
    inFile >> numVehicles;
    inFile >> numRides;
    inFile >> bonus;
    inFile >> numSteps;


    std::vector<Car*>* cars = new std::vector<Car*>;
    int carId = 1;
    for (int i = 0; i < numVehicles; i++) {
        cars->push_back(new Car(0,0, carId++));
    }

    // std::cout << rows << " " <<  cols << " ";
    // std::cout << numVehicles << " " <<  numRides << " ";
    // std::cout << bonus << " " <<  numSteps << "\n";

    std::vector<Passenger *> passengers = readInFile(inFile, rows, cols);

    int T = 0;

    for (int i = 0; i < passengers.size(); i++) {
        passengers[i]->carList = cars;
    }

    std::list<std::pair<Passenger*, Car*>> moving;
    int carIndex;
    while (T <= numSteps) {

        
        for (int i = 0; i < passengers.size(); i++) {
            if (!passengers[i]->inTransist) {
                carIndex = passengers[i]->FindCar(T);
                if (carIndex >= 0){
                    // cout << "========-Checking car: " << carIndex << "========-\n";
                    moving.push_back(std::make_pair(passengers[i], cars->at(carIndex)));
                    passengers[i]->inTransist = true;
                    cars->at(carIndex)->available = false;
                    passengers.erase(passengers.begin() + i);
                    cars->erase(cars->begin() + carIndex);
                } else {
                    // cout << "-------No car found for passenger " << passengers[i]->id << "-------\n";
                }
            }
        }

        for (auto c_p_pair : moving) {
            // cout << "||||||||Checking pair: " << c_p_pair.first->id << "||||||||\n";
            if (c_p_pair.first->Move()) {

                cars->push_back(c_p_pair.second);
                delete c_p_pair.first;
            }            
        }
        T++;
    }



    for (auto car: *cars) {

        // cout << "id:" <<  car->id << " -  ";
        // outFile << car->id << " ";
        outFile << car->passengersMoved.size() << " ";
        for (auto pass : car->passengersMoved) {
            // cout << pass << " ";
            outFile << pass << " ";
        }
        // cout << "\n";
        outFile << "\n";
    }
    outFile.close();

    return 0;

   
}

