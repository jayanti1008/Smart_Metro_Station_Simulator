#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define DAY_MINUTES 1440

// --------------------------- Data Structures ---------------------------

typedef struct {
    int tokenID;
    char name[30];
    int priority; // 1 = VIP/Emergency, 0 = Normal
} Passenger;

// Priority Queue
Passenger queue[MAX];
int front = -1, rear = -1;

// Stack for cancelled tickets
Passenger stack[MAX];
int top = -1;

// --------------------------- Metro Stations ---------------------------

// Purple Line
char *purpleLineStations[] = {
    "Whitefield","Hopefarm Channasandra","Kadugodi Tree Park",
    "Pattanduru Agrahara","Sri Sathya Sai Hospital","Nallurhalli",
    "Kundalahalli","Seetharamapalya","Hoodi Junction",
    "Garudachar Palya","Singayyanapalya","Krishna Raja Pura",
    "Benniganahalli","Baiyappanahalli","Swami Vivekananda Road",
    "Indiranagar","Halasuru","Trinity","Mahatma Gandhi Road",
    "Cubbon Park","Dr. B.R. Ambedkar Stn, Vidhana Soudha",
    "Sir M. Visveswaraya Stn, Central College",
    "Nadaprabhu Kempegowda Stn, Majestic",
    "Krantivira Sangolli Rayanna Railway Stn","Magadi Road",
    "Hosahalli","Vijayanagar","Attiguppe","Deepanjali Nagar",
    "Mysuru Road","Nayandahalli","Rajarajeshwari Nagar",
    "Jnanabharathi","Pattanagere","Kengeri Bus Terminal",
    "Kengeri","Challaghatta"
};
int purpleLineCount = 37;

// Green Line
char *greenLineStations[] = {
    "Madavara","Chikkabidarakallu","Manjunathanagar","Nagasandra",
    "Dasarahalli","Jalahalli","Peenya Industry","Peenya",
    "Goraguntepalya","Yeshwantpur","Sandal Soap Factory",
    "Mahalakshmi","Rajajinagar","Mahakavi Kuvempu Road",
    "Srirampura","Mantri Square Sampige Road",
    "Nadaprabhu Kempegowda Stn, Majestic","Chickpete",
    "Krishna Rajendra Market","National College","Lalbagh",
    "South End Circle","Jayanagar",
    "Rashtreeya Vidyalaya Road","Banashankari",
    "Jaya Prakash Nagar","Yelachenahalli",
    "Konanakunte Cross","Doddakallasandra",
    "Vajarahalli","Thalaghattapura","Silk Institute"
};
int greenLineCount = 32;

// Yellow Line
char *yellowLineStations[] = {
    "RV Road","Ragigudda","Jayadeva Hospital","BTM Layout",
    "Central Silk Board","Bommanahalli","Hongasandra",
    "Kudlu Gate","Singasandra","Hosa Road",
    "Beratena Agrahara","Electronic City",
    "Infosys Foundation Konappana Agrahara",
    "Huskur Road","Biocon Hebbagodi",
    "Delta Electronics Bommasandra"
};
int yellowLineCount = 16;

// --------------------------- Helper Functions ---------------------------

void printTime(int minutes) {
    int hour = (minutes / 60) % 24;
    int min = minutes % 60;
    char period[3] = "AM";

    if (hour >= 12) strcpy(period, "PM");
    if (hour == 0) hour = 12;
    else if (hour > 12) hour -= 12;

    printf("%02d:%02d %s", hour, min, period);
}

int calculateArrival(int currentTime, int stationIndex) {
    return (currentTime + stationIndex * 2) % DAY_MINUTES;
}

// --------------------------- Queue Functions ---------------------------

void enqueue(Passenger p) {
    if ((rear + 1) % MAX == front) {
        printf("Queue is full!\n");
        return;
    }

    if (front == -1) {
        front = rear = 0;
        queue[rear] = p;
        return;
    }

    int i = rear;
    rear = (rear + 1) % MAX;

    if (p.priority == 1) { // VIP priority
        while (i != front && queue[i].priority == 0) {
            queue[(i + 1) % MAX] = queue[i];
            i = (i - 1 + MAX) % MAX;
        }
        queue[(i + 1) % MAX] = p;
    } else {
        queue[rear] = p;
    }

    printf("Passenger %s added (Token %d, Priority %d)\n",
           p.name, p.tokenID, p.priority);
}

Passenger dequeue() {
    Passenger temp;
    temp.tokenID = -1;

    if (front == -1)
        return temp;

    temp = queue[front];

    if (front == rear)
        front = rear = -1;
    else
        front = (front + 1) % MAX;

    return temp;
}

// --------------------------- Stack Functions ---------------------------

void pushStack(Passenger p) {
    if (top < MAX - 1)
        stack[++top] = p;
}

Passenger popStack() {
    Passenger temp;
    temp.tokenID = -1;

    if (top == -1)
        return temp;

    return stack[top--];
}

// --------------------------- Passenger Operations ---------------------------

int nextToken = 101;

void addPassenger() {
    Passenger p;

    printf("\nEnter passenger name: ");
    scanf(" %[^\n]", p.name);

    printf("Enter priority (1 = VIP, 0 = Normal): ");
    scanf("%d", &p.priority);

    p.tokenID = nextToken++;
    enqueue(p);
}

void bookTicket() {
    Passenger p = dequeue();
    if (p.tokenID == -1) {
        printf("No passengers waiting.\n");
        return;
    }
    printf("Ticket booked for %s (Token %d)\n", p.name, p.tokenID);
}

void cancelTicket() {
    Passenger p = dequeue();
    if (p.tokenID == -1) {
        printf("No passengers to cancel.\n");
        return;
    }
    pushStack(p);
    printf("Ticket cancelled for %s (Token %d)\n", p.name, p.tokenID);
}

void undoCancellation() {
    Passenger p = popStack();
    if (p.tokenID == -1) {
        printf("No cancellations to undo.\n");
        return;
    }
    enqueue(p);
    printf("Cancellation undone for %s (Token %d)\n", p.name, p.tokenID);
}

// --------------------------- Train Arrival ---------------------------

void showTrainArrival() {
    char station[50];
    int hour, minute;
    int currentTime;
    int found = 0;

    printf("\nEnter station name: ");
    scanf(" %[^\n]", station);

    printf("Enter current time (HH:MM): ");
    scanf("%d:%d", &hour, &minute);

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        printf("Invalid time format!\n");
        return;
    }

    currentTime = hour * 60 + minute;

    printf("\nUpcoming trains at %s:\n", station);

    // Purple Line
    for (int i = 0; i < purpleLineCount; i++) {
        if (strcmp(purpleLineStations[i], station) == 0) {
            found = 1;
            printf("Purple Line:\n");
            for (int t = 0; t < 3; t++) {
                int arr = calculateArrival(currentTime + t * 7, i);
                printf("  Train %d at ", t + 1);
                printTime(arr);
                printf("\n");
            }
        }
    }

    // Green Line
    for (int i = 0; i < greenLineCount; i++) {
        if (strcmp(greenLineStations[i], station) == 0) {
            found = 1;
            printf("Green Line:\n");
            for (int t = 0; t < 3; t++) {
                int arr = calculateArrival(currentTime + t * 7, i);
                printf("  Train %d at ", t + 1);
                printTime(arr);
                printf("\n");
            }
        }
    }

    // Yellow Line
    for (int i = 0; i < yellowLineCount; i++) {
        if (strcmp(yellowLineStations[i], station) == 0) {
            found = 1;
            printf("Yellow Line:\n");
            for (int t = 0; t < 3; t++) {
                int arr = calculateArrival(currentTime + t * 7, i);
                printf("  Train %d at ", t + 1);
                printTime(arr);
                printf("\n");
            }
        }
    }

    if (!found)
        printf("Station not found in metro lines.\n");
}

// --------------------------- Boarding ---------------------------

void simulateBoarding() {
    Passenger p = dequeue();
    if (p.tokenID == -1) {
        printf("No passengers to board.\n");
        return;
    }
    printf("Passenger %s (Token %d) boarded successfully.\n",
           p.name, p.tokenID);
}

// --------------------------- Main ---------------------------

int main() {
    int choice;

    while (1) {
        printf("\nSMART METRO STATION SIMULATOR\n");
        printf("1. Add Passenger\n");
        printf("2. Book Ticket\n");
        printf("3. Cancel Ticket\n");
        printf("4. Undo Cancellation\n");
        printf("5. Show Train Arrival\n");
        printf("6. Simulate Boarding\n");
        printf("7. Exit\n");
        printf("Enter choice: ");

        scanf("%d", &choice);
        printf("\n");

        switch (choice) {
            case 1: addPassenger(); break;
            case 2: bookTicket(); break;
            case 3: cancelTicket(); break;
            case 4: undoCancellation(); break;
            case 5: showTrainArrival(); break;
            case 6: simulateBoarding(); break;
            case 7: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
}
