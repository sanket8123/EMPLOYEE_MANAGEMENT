#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "employees.dat"
#define NAME_LEN 64
#define DEPT_LEN 48
#define ROLE_LEN 48

typedef struct {
    int id;
    char name[NAME_LEN];
    char department[DEPT_LEN];
    char role[ROLE_LEN];
    double salary;
} Employee;

static void clear_input(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

static void read_line(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static int read_int(const char *prompt) {
    int value;
    int result;

    while (1) {
        printf("%s", prompt);
        result = scanf("%d", &value);
        clear_input();

        if (result == 1) {
            return value;
        }

        printf("Invalid input. Please enter a number.\n");
    }
}

static double read_double(const char *prompt) {
    double value;
    int result;

    while (1) {
        printf("%s", prompt);
        result = scanf("%lf", &value);
        clear_input();

        if (result == 1 && value >= 0) {
            return value;
        }

        printf("Invalid input. Please enter a positive amount.\n");
    }
}

static FILE *open_file(const char *mode) {
    FILE *file = fopen(DATA_FILE, mode);
    if (file == NULL) {
        printf("Could not open data file.\n");
    }
    return file;
}

static int id_exists(int id) {
    Employee employee;
    FILE *file = fopen(DATA_FILE, "rb");

    if (file == NULL) {
        return 0;
    }

    while (fread(&employee, sizeof(Employee), 1, file) == 1) {
        if (employee.id == id) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

static void print_header(void) {
    printf("\n%-8s %-24s %-18s %-18s %12s\n", "ID", "Name", "Department", "Role", "Salary");
    printf("--------------------------------------------------------------------------------------\n");
}

static void print_employee(const Employee *employee) {
    printf("%-8d %-24s %-18s %-18s %12.2f\n",
           employee->id,
           employee->name,
           employee->department,
           employee->role,
           employee->salary);
}

static void add_employee(void) {
    Employee employee;
    FILE *file;

    printf("\nAdd Employee\n");
    employee.id = read_int("Employee ID: ");

    if (id_exists(employee.id)) {
        printf("An employee with this ID already exists.\n");
        return;
    }

    read_line("Name: ", employee.name, NAME_LEN);
    read_line("Department: ", employee.department, DEPT_LEN);
    read_line("Role: ", employee.role, ROLE_LEN);
    employee.salary = read_double("Salary: ");

    file = open_file("ab");
    if (file == NULL) {
        return;
    }

    fwrite(&employee, sizeof(Employee), 1, file);
    fclose(file);
    printf("Employee added successfully.\n");
}

static void list_employees(void) {
    Employee employee;
    int count = 0;
    FILE *file = fopen(DATA_FILE, "rb");

    if (file == NULL) {
        printf("\nNo employee records found.\n");
        return;
    }

    print_header();
    while (fread(&employee, sizeof(Employee), 1, file) == 1) {
        print_employee(&employee);
        count++;
    }

    fclose(file);

    if (count == 0) {
        printf("No employee records found.\n");
    } else {
        printf("\nTotal employees: %d\n", count);
    }
}

static void search_employee(void) {
    Employee employee;
    int id = read_int("\nEnter employee ID to search: ");
    FILE *file = fopen(DATA_FILE, "rb");

    if (file == NULL) {
        printf("No employee records found.\n");
        return;
    }

    while (fread(&employee, sizeof(Employee), 1, file) == 1) {
        if (employee.id == id) {
            print_header();
            print_employee(&employee);
            fclose(file);
            return;
        }
    }

    fclose(file);
    printf("Employee not found.\n");
}

static void update_employee(void) {
    Employee employee;
    int id = read_int("\nEnter employee ID to update: ");
    int found = 0;
    FILE *file = open_file("rb+");

    if (file == NULL) {
        return;
    }

    while (fread(&employee, sizeof(Employee), 1, file) == 1) {
        if (employee.id == id) {
            printf("\nCurrent details:\n");
            print_header();
            print_employee(&employee);

            printf("\nEnter new details\n");
            read_line("Name: ", employee.name, NAME_LEN);
            read_line("Department: ", employee.department, DEPT_LEN);
            read_line("Role: ", employee.role, ROLE_LEN);
            employee.salary = read_double("Salary: ");

            fseek(file, -(long)sizeof(Employee), SEEK_CUR);
            fwrite(&employee, sizeof(Employee), 1, file);
            found = 1;
            break;
        }
    }

    fclose(file);
    printf(found ? "Employee updated successfully.\n" : "Employee not found.\n");
}

static void delete_employee(void) {
    Employee employee;
    int id = read_int("\nEnter employee ID to delete: ");
    int found = 0;
    FILE *source = fopen(DATA_FILE, "rb");
    FILE *target;

    if (source == NULL) {
        printf("No employee records found.\n");
        return;
    }

    target = fopen("employees.tmp", "wb");
    if (target == NULL) {
        fclose(source);
        printf("Could not create temporary file.\n");
        return;
    }

    while (fread(&employee, sizeof(Employee), 1, source) == 1) {
        if (employee.id == id) {
            found = 1;
        } else {
            fwrite(&employee, sizeof(Employee), 1, target);
        }
    }

    fclose(source);
    fclose(target);

    if (remove(DATA_FILE) != 0 || rename("employees.tmp", DATA_FILE) != 0) {
        printf("Could not update employee records.\n");
        return;
    }

    printf(found ? "Employee deleted successfully.\n" : "Employee not found.\n");
}

static void show_menu(void) {
    printf("\nEmployee Management System\n");
    printf("1. Add employee\n");
    printf("2. List employees\n");
    printf("3. Search employee\n");
    printf("4. Update employee\n");
    printf("5. Delete employee\n");
    printf("0. Exit\n");
}

int main(void) {
    int choice;

    do {
        show_menu();
        choice = read_int("Choose an option: ");

        switch (choice) {
            case 1:
                add_employee();
                break;
            case 2:
                list_employees();
                break;
            case 3:
                search_employee();
                break;
            case 4:
                update_employee();
                break;
            case 5:
                delete_employee();
                break;
            case 0:
                printf("Goodbye.\n");
                break;
            default:
                printf("Invalid option. Please try again.\n");
        }
    } while (choice != 0);

    return 0;
}
