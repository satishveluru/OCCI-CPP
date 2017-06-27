/**
 * occipobj.cpp - Manipulation (Insertion, selection & updating) of 
 *                persistant objects, along with pinning, unpinning, marking 
 *                for deletion & flushing.
 *
 * Description
 *   Create a program which has insert, select, update & delete as operations
 *   of a persistant object.  Along with the these the operations on Ref. are
 *   pinning, unpinning, marked for deletion & flushing. 
 *   
 */

#include <iostream.h>
#include <occi.h>
using namespace oracle::occi;
using namespace std;

#include "occipobjm.h"

class address_obj : public address 
{
  public: 
   address_obj()
   {
   }
   
   address_obj(Number sno,string cty)
   {
     setStreet_no(sno);
     setCity(cty);
   }
};

class occipobj
{
  private:

  Environment *env;
  Connection *conn;
  Statement *stmt;
  string tableName;
  string typeName;

  public:

  occipobj (string user, string passwd, string db)
  {
    env = Environment::createEnvironment (Environment::OBJECT);
    occipobjm (env);
    conn = env->createConnection (user, passwd, db);
  }

  ~occipobj ()
  {
    env->terminateConnection (conn);
    Environment::terminateEnvironment (env);
  }

  void setTableName (string s)
  {
    tableName = s;
  }

  /**
   * Insertion of a row 
   */
  void insertRow (int a1, string a2)
  {
    cout << "Inserting row ADDRESS (" << a1 << ", " << a2 << ")" << endl;
    Number n1(a1);
    address_obj *o = new (conn, tableName) address_obj(n1, a2);
    conn->commit ();
    cout << "Insertion - Successful" << endl;
  }

  /**
   * updating a row
   */
  void updateRow (int b1, int a1, string a2)
  {
    cout << "Updating a row with attribute a1 = " << b1 << endl;
    stmt = conn->createStatement
      ("SELECT REF(a) FROM address_tab a WHERE street_no = :x FOR UPDATE");
    stmt->setInt (1, b1);
    ResultSet *rs = stmt->executeQuery ();
    try{
    if ( rs->next() )
    {
      RefAny rany = rs->getRef (1);
      Ref <address_obj > r1(rany);
      address_obj *o = r1.ptr();
      o->markModified ();
      o->setStreet_no (Number (a1));
      o->setCity (a2);
      o->flush ();
    }
    }catch(SQLException ex)
    {
     cout<<"Exception thrown updateRow"<<endl;
     cout<<"Error number: "<<  ex.getErrorCode() << endl;
     cout<<ex.getMessage() << endl;
    }

    conn->commit ();
    conn->terminateStatement (stmt);
    cout << "Updation - Successful" << endl;
  }


  /**
   * deletion of a row
   */
  void deleteRow (int a1, string a2)
  {
    cout << "Deleting a row with object ADDRESS (" << a1 << ", " << a2 
     << ")" << endl;
    stmt = conn->createStatement
    ("SELECT REF(a) FROM address_tab a WHERE street_no = :x AND city = :y FOR 
UPDATE");
    stmt->setInt (1, a1);
    stmt->setString (2, a2);
    ResultSet *rs = stmt->executeQuery ();
    try{
    if ( rs->next() )
    {
      RefAny rany = rs->getRef (1);
      Ref<address_obj > r1(rany);
      address_obj *o = r1.ptr();
      o->markDelete ();
    }
    }catch(SQLException ex)
    {
     cout<<"Exception thrown for deleteRow"<<endl;
     cout<<"Error number: "<<  ex.getErrorCode() << endl;
     cout<<ex.getMessage() << endl;
    }

    conn->commit ();
    conn->terminateStatement (stmt);
    cout << "Deletion - Successful" << endl;
  }

  /**
   * displaying all the rows in the table
   */
  void displayAllRows ()
  {
    string sqlStmt = "SELECT REF (a) FROM address_tab a";
    stmt = conn->createStatement (sqlStmt);
    ResultSet *rset = stmt->executeQuery ();
    try{
    while (rset->next ())
    {
      RefAny rany = rset->getRef (1); 
      Ref<address_obj > r1(rany);
      address_obj *o = r1.ptr();
      cout << "ADDRESS(" << (int)o->getStreet_no () << ", " << o->getCity () << 
")" << endl;
    }
    }catch(SQLException ex)
    {
     cout<<"Exception thrown for displayAllRows"<<endl;
     cout<<"Error number: "<<  ex.getErrorCode() << endl;
     cout<<ex.getMessage() << endl;
    }

    stmt->closeResultSet (rset);
    conn->terminateStatement (stmt);
  }

}; // end of class occipobj


int main (void)
{
  string user = "SCOTT";
  string passwd = "TIGER";
  string db = "";

  try
  {
    cout << "occipobj - Exhibiting simple insert, delete & update operations" 
     " on persistent objects" << endl;
    occipobj *demo = new occipobj (user, passwd, db);

    cout << "Displaying all rows before the opeations" << endl;
    demo->displayAllRows ();

    demo->setTableName ("ADDRESS_TAB");
 
    demo->insertRow (21, "KRISHNA");

    demo->deleteRow (22, "BOSTON");
  
    demo->updateRow (33, 123, "BHUMI");
  
    cout << "Displaying all rows after all the operations" << endl;
    demo->displayAllRows ();

    delete (demo);
    cout << "occipobj - done" << endl;
  }catch (SQLException ea)
  {
    cerr << "Error running the demo: " << ea.getMessage () << endl;
  }
}
