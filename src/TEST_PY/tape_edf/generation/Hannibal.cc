
#include "hexa_base.hxx"
#include <map>

//--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8
enum EnumItem  { IT_Ident, IT_Shape, IT_Type,   IT_Subid,
                 IT_Face,  IT_Edge,  IT_Vertex, IT_Coord, IT_End };
enum EnumClass { CL_Alph, CL_Num, CL_2pts, CL_Pnt, CL_Virg, CL_Parg, CL_Pard,
                 CL_Other, CL_Max };
enum EnumState { ST_Nada, ST_Ident, ST_Coor, ST_End, ST_Max };

cpchar Barre     = "===============================";
cpchar proc_name = "build";

// ========================================================= Hannibal
class Hannibal
{
public :
   Hannibal ();
   void analyze (cpchar filename);

private :
   EnumItem   getItem  (string& word);
   EnumClass  getClass (int carac);
   void purge ();
   void openProcedure (rcstring name);
   void openHexa      (rcstring name);
   void openQuad      (rcstring name);
   void openEdge      (rcstring name);
   void openVertex    (rcstring name);
   void setCoord      (rcstring coord);
   void printProcedure ();
   void mainProcedure ();
   void printLine  (cpchar fonc, rcstring left, rcstring right);
   void printEdge  (rcstring left, rcstring right);
   void printPoint (rcstring right);
   void print (cpchar m1="", cpchar m2="", cpchar m3="", cpchar m4="");

private :
   FILE*  fic_log;
   FILE*  fic_proc;
   string geo_name;

   vector <string>     tab_geom;
   map <string,string> map_vertex;
   map <string,string> map_edge;
   map <string,string> map_quad;
   map <string,string> map_hexa;

   string cur_vertex, cur_edge, cur_quad, cur_hexa;
   string def_vertex, def_edge, def_quad, def_hexa;
   int    nbr_vertex, nbr_edge, nbr_quad, nbr_hexa;

   enum EnumKind { KindNone, KindCylinder, KindVis };
   EnumKind geo_kind;
};
// ========================================================= Constructeur
Hannibal::Hannibal ()
{
   fic_log   = NULL;
   fic_proc  = NULL;
   purge ();
}
// ========================================================= purge
void Hannibal::purge ()
{
   geo_name    = "";
   geo_kind = KindNone;

   map_vertex.clear();
   map_edge  .clear();
   map_quad  .clear();
   map_hexa  .clear();

   cur_vertex = cur_edge = cur_quad = cur_hexa = "";
   def_vertex = def_edge = def_quad = def_hexa = "";
   nbr_vertex = nbr_edge = nbr_quad = nbr_hexa = 0;
}
// ========================================================= getClass
EnumClass Hannibal::getClass (int car)
{
   if (isalpha (car)) return CL_Alph;
   if (isdigit (car)) return CL_Num;
   if (car==':')      return CL_2pts;
   if (car==',')      return CL_Virg;
   if (car=='(')      return CL_Parg;
   if (car==')')      return CL_Pard;
   if (car=='_')      return CL_Num;
   if (car=='-')      return CL_Num;
   if (car=='=')      return CL_Num;
   if (car=='.')      return CL_Pnt;
   return CL_Other;

}
// ========================================================= getItem
EnumItem Hannibal::getItem (string& word)
{
static EnumState transition [ST_End] [CL_Max] = {
//CL_Alph   CL_Num    CL_2pts   CL_Pnt   CL_Virg  CL_Parg  CL_Pard  CL_Other
{ ST_Ident, ST_Nada,  ST_Nada,  ST_Nada, ST_Nada, ST_Coor, ST_Nada, ST_Nada},
{ ST_Ident, ST_Ident, ST_Ident, ST_End,  ST_End,  ST_End,  ST_End,  ST_End },
{ ST_Coor,  ST_Coor,  ST_End,   ST_Coor, ST_Coor, ST_End,  ST_End,  ST_End }};

   word = "";
   EnumState etat = ST_Nada;
   while (true)
         {
         EnumState petat = etat;
         int       carac = fgetc (fic_log);
         if (carac==EOF)
             return IT_End;
         EnumClass classe = getClass (carac);
         etat = transition [etat][classe];
         switch (etat)
            {
            case ST_Ident :
                 if (carac==':')
                    word = "";
                 else
                    word += carac;
                 break;

            case ST_Coor :
                 word += carac;
                 break;

            case ST_End :
                 if (petat==ST_Coor)
                    {
                    word += ")";
                    return IT_Coord;
                    }
                 else if (word == "setShape")
                    return IT_Shape;
                 else if (word.find ("vertex_")==0)
                    return IT_Vertex;
                 else if (word.find ("type=")==0)
                    return IT_Type;
                 else if (word.find ("subid=")==0)
                    return IT_Subid;
                 else if (word.find ("face_")==0)
                    return IT_Face;
                 else if (word.find ("edge_")==0)
                    return IT_Edge;
                 else
                    return IT_Ident;
                 break;
            default: ;
            }
         }
}
// ========================================================= openProcedure
void Hannibal::openProcedure (rcstring name)
{
   if (nbr_hexa>0)
      printProcedure ();

   purge ();
   geo_name = name;
   tab_geom.push_back (geo_name);

}
// ========================================================= openHexa
void Hannibal::openHexa (rcstring name)
{
   nbr_hexa ++;
   nbr_quad = 0;
   cur_hexa = name;
   cur_hexa.replace (0,6, "hexa_");
   def_hexa = "(";
}
// ========================================================= openQuad
void Hannibal::openQuad (rcstring name)
{
   nbr_edge = 0;
   cur_quad = name;
   def_quad = "(";

   nbr_quad++;
   if (nbr_quad<6)
      def_hexa += name + ", ";
   else
      {
      def_hexa += name + ")";
      map_hexa [cur_hexa] = def_hexa;
      }
}
// ========================================================= openEdge
void Hannibal::openEdge (rcstring name)
{
   nbr_vertex = 0;
   cur_edge   = name;
   def_edge   = "(";

   nbr_edge++;
   if (nbr_edge<4)
       def_quad += name + ", ";
   else
      {
      def_quad += name + ")";
      map_quad [cur_quad] = def_quad;
      }
}
// ========================================================= openVertex
void Hannibal::openVertex (rcstring name)
{
   cur_vertex = name;
   nbr_vertex++;
   if (nbr_vertex==1)
       def_edge += name + ", ";
   else
      {
      def_edge += name + ")";
      map_edge [cur_edge] = def_edge;
      }
}
// ========================================================= setCoord
void Hannibal::setCoord (rcstring coord)
{
   map_vertex [cur_vertex] = coord;
}
// ========================================================= print
void Hannibal::print (cpchar m1, cpchar m2, cpchar m3, cpchar m4)
{
   fprintf (fic_proc, m1, m2, m3, m4);
   fprintf (fic_proc, "\n");
}
// ========================================================= printEdge
void Hannibal::printEdge (rcstring left, rcstring right)
{
   cpchar droite = right.c_str();
   cpchar sub    = left .c_str();
   sub    = &sub   [5];
   if (sub[0] == '0' && strlen(sub)>1)
       sub = &sub [1];

   droite = &droite[1];
   print ("   %s = add_edge (%s, %s", left.c_str(),  sub, droite);
}
// ========================================================= printLine
void Hannibal::printLine (cpchar fonc, rcstring left, rcstring right)
{
   print ("   %s = doc.%s %s", left.c_str(),  fonc, right.c_str());
}
// ========================================================= printPoint
void Hannibal::printPoint (rcstring coord)
{
   print ("   add_point %s", coord.c_str());
}
// ========================================================= printProcedure
void Hannibal::printProcedure ()
{
   string proc    = "make" + geo_name;
   string proc_py = proc + ".py";

   cpchar geom      = geo_name.c_str();
// cpchar proc_name = proc.c_str();
   cpchar fic_name  = proc_py.c_str();

   cout << " ... Generation de " << fic_name << endl;
   fic_proc = fopen (fic_name, "w");
   print ("");
   print ("# Hexa : Creation de %s", geom);
   print ("");
   print ("import hexablock");
   print ("from hexa_tape import *");

   cpchar  load    = "True";
   cpchar  genproc = "gen_cylinder";
   if (strncmp(geom,"Plaq_",5)==0 || strncmp(geom,"Rond_",5)==0)
      {
      geo_kind = KindCylinder;
      load    = "False";
      genproc = "gen_cylinder";
      }
   else if (strncmp(geom,"VisA_",5)==0 || strncmp(geom,"VisB_",5)==0)
      {
      geo_kind = KindVis;
      genproc  = "gen_vis";
      load     = "False";
      }
   else
      {
      geo_kind = KindNone;
      load = "True";
      }

   print ("");
   print ("# %s%s %s", Barre, Barre, proc_name);
   print ("def %s () :", proc_name);
   print ("   name  = '%s'", geom);
   print ("   geom  = '%s'", load);
   print ("   doc   = add_document (name, geom)");
   print ("");

   map <string,string> :: iterator iter;
   if (geo_kind != KindNone)
      {
      print ("   raz_points ()");
      for (iter = map_vertex.begin() ; iter != map_vertex.end() ; ++iter)
          printPoint (iter->second);
      print ("   %s ()", genproc);
      }
   else
      {
      for (iter = map_vertex.begin() ; iter != map_vertex.end() ; ++iter)
          {
          printLine ("addVertex", iter->first,  iter->second);
          }
      print ("");
      for (iter = map_edge.begin() ; iter != map_edge.end() ; ++iter)
          {
          printEdge (iter->first,  iter->second);
          }

      print ("");
      for (iter = map_quad.begin() ; iter != map_quad.end() ; ++iter)
          {
          printLine ("addQuad", iter->first,  iter->second);
          }

      print ("");
      for (iter = map_hexa.begin() ; iter != map_hexa.end() ; ++iter)
          {
          printLine ("addHexa", iter->first,  iter->second);
          }
      }

   print  ("   return doc");

   print ("");
   print ("# %s%s %s", Barre, Barre, "Begin");
   print ("");

   print ("if __name__ == '__main__' :");
   print ("   doc = %s ()", proc_name);
   print ("   mesh_hexas = mailler ()");

   fclose (fic_proc);
   fic_proc = NULL;
}
// ========================================================= mainProcedure
void Hannibal::mainProcedure ()
{
   if (nbr_hexa>0)
      printProcedure ();

   cpchar fic_name  = "assemble_tape.py";
   cout << " ... Generation de " << fic_name << endl;

   fic_proc = fopen (fic_name, "w");
   print ("");
   print ("# Hexa : Assemblage des composants de la tape %s");
   print ("");
   print ("import hexablock");
   print ("import smesh");
   print ("from hexa_tape import *");


   int nbre = tab_geom.size();
   print  ("");
   for (int nro=0 ; nro<nbre ; ++nro)
       {
       cpchar geom = tab_geom[nro].c_str();
       cpchar plus = "";
       if (Cestegal (geom, "VisC_4"))
          {
          geom =  "_vis_c";
          plus =  "      ### not generated";
          }
       print ("import make%s", geom, plus);
       }

   print ("");
   print ("# %s%s %s", Barre, Barre, "Begin");
   print ("");
   print ("tab_piece = []");
   for (int nro=0 ; nro<nbre ; ++nro)
       {
       cpchar geom = tab_geom[nro].c_str();
       cpchar plus = "";
       if (Cestegal (geom, "VisC_4"))
          {
          geom =  "_vis_c";
          plus =  "      ### not generated";
          }
       print ("tab_piece.append (make%s.%s ())%s", geom, 
                                 proc_name,plus);
       }
   print ("");
   print ("tab_mesh  = []");
   print ("for doc in tab_piece :");
   print ("    print ' ... Maillage de ', doc.getName()");

   print ("    mesh = mailler (doc)");
   print ("    tab_mesh.append (mesh)");

   print ("");
   print ("global_mesh = smesh.Concatenate (tab_mesh, %s)",
                         "False, False, allGroups=False");
}
// ========================================================= analyze
void Hannibal::analyze (cpchar name)
{
   fic_log = fopen (name, "r");
   if (fic_log==NULL)
      {
      cout << " *** " << endl;
      cout << " *** Fichier log " << name << " Inaccessible" << endl;
      cout << " *** " << endl;
      return;
      }

   string word;
   bool   isashape = false;
   bool   isanhexa = false;
   while (true)
         {
         EnumItem item = getItem (word);
         switch (item)
            {
            case IT_Shape :
                 isashape = true;
                 break;
            case IT_Ident :
                 if (isashape)
                    {
                    openProcedure (word);
                    }
                 isashape = false;
                 break;

            case IT_Type :
                 isanhexa = word=="type=3";
                 break;
            case IT_Subid :
                 if (isanhexa)
                    openHexa (word);
                 isanhexa = false;
                 break;
            case IT_Face :
                 openQuad (word);
                 break;
            case IT_Edge :
                 openEdge (word);
                 break;
            case IT_Vertex :
                 openVertex (word);
                 break;
            case IT_Coord :
                 setCoord (word);
                 break;
            case IT_End   :
                 mainProcedure ();
                 return;
                 break;
            }
         }
}
// ======================================================== Main
int main ()
{
   Hannibal lecteur;
   lecteur.analyze ("load_brep.log");
}
