//
// Created by christou on 04/25.
//
#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cstdlib>
#include<cmath>
#include<nlohmann/json.hpp>
using json = nlohmann::json;
typedef struct{
  std::string titre;
  std::string auteur;
  std::string genre;
  int annee;
  double prix;
  bool diponible;
  }Livre;
  int main(int argc, char *argv[]){
    std::ifstream jfile("./resources/librairie.database");

    if (!jfile || jfile.peek() == std::ifstream::traits_type::eof()) {
      std::cerr << "Erreur : fichier JSON introuvable ou vide." << std::endl;
      return 1;
    }
    std::vector<Livre> livres;
    Livre livre;
    livre.titre="Une brève histoire du temps";
    livre.genre="science";
    livre.genre="science";
    livre.annee=1988;
    livre.prix=46.39;
    livre.diponible=true;
    json jlivre ;
    /*Serialisation basique*/
    //creation d'objet database
    jlivre["titre"]=livre.titre;
    jlivre["auteur"]=livre.auteur;
    jlivre["genre"]=livre.genre;
    jlivre["annee"]=livre.annee;
    jlivre["prix"]=livre.prix;
    jlivre["diponible"]=livre.diponible;
    livres.push_back(livre);
    //Conversion de l'Objet en string
    std::string json_string = jlivre.dump();
    std::cout << json_string << std::endl;
    /*Decerialisation Conversion de l'OBJET Json en structure c++ */
    json livre_parsed;
    jfile >> livre_parsed;
    std::cout << livre_parsed["livres"][1]["titre"] << std::endl;
    }
    using json = nlohmann::json;
    typedef struct{
      std::string titre;
      std::string auteur;
      std::string genre;
      int annee;
      double prix;
      bool diponible;
      }Livre;
      json jlivre ;
      int main(){
        Livre livre ={"L'Étranger","Albert Camus","Philosophie",1942, 47.39,true};
        /*Serialisation basique creation d'objet database*/
        jlivre["titre"]=livre.titre;
        jlivre["auteur"]=livre.auteur;
        jlivre["genre"]=livre.genre;
        jlivre["annee"]=livre.annee;
        jlivre["prix"]=livre.prix;
        jlivre["diponible"]=livre.diponible;
        /*conversion de lObjet en en string*/
        std::string json_string = jlivre.dump(5);
        std::cout << json_string << std::endl;
        /*Décerialisation conversion de Javascript Objet en Structure de données C++*/
        //database jlivre = database::parse(json_string);
        jlivre["titre"]=livre.titre;
        jlivre["auteur"]=livre.auteur;
        jlivre["genre"]=livre.genre;
        jlivre["annee"]=livre.annee;
        jlivre["prix"]=livre.prix;
        jlivre["diponible"]=livre.diponible;
        std::cout<< "Titre : "<<livre.titre<<" Auteur : "<<livre.auteur<<std::endl;
      }
      /*
/*SQL
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
      int main() {
        try {
          sql::Driver *driver;
          sql::Connection *con;
          sql::Statement *stmt;
          sql::ResultSet *res; // Create a connection
          driver = get_driver_instance();
          con = driver->connect("tcp://127.0.0.1:3306", "user", "password");
          con->setSchema("mydatabase");
          // Create a statement
          stmt = con->createStatement();
          res = stmt->executeQuery("SELECT * FROM users");
          // Process the result set
          while (res->next()) {
            std::cout << res->getString("name") << " - " << res->getString("email") << std::endl;
          }
          delete res;
          delete stmt;
          delete con;
        } catch (sql::SQLException &e) {
          std::cerr << "# ERR: SQLException in " << __FILE__;
          std::cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
          std::cerr << "# ERR: " << e.what();
          std::cerr << " (MySQL error code: " << e.getErrorCode();
          std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        }
      }
#include <pqxx/pqxx>
      int main() {
        try {
          pqxx::connection C("dbname=mydatabase user=user password=password");
          pqxx::work W(C);
          pqxx::result R = W.exec("SELECT * FROM users");
          for (auto row: R) { std::cout << row["name"].c_str() << " - " << row["email"].c_str() << std::endl;
          } W.commit();
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
        }
      }
*/