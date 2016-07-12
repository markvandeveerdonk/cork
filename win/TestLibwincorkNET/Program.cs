using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using WincorkNET;

namespace TestLibwincorkNET
{
    class Program
    {
        const int ID1 = 1;
        const int ID2 = 2;
        const int Out1 = 3;
        const int Out2 = 4;

        static void Assert(bool f)
        {
            if (!f) {
                Console.Error.WriteLine("Fail!");
                Environment.Exit(0);
            }
        }

        static void Main(string[] args)
        {
            if (!Cork.LoadMesh(ID1, "..\\..\\..\\..\\samples\\ballA.off")) {
                Console.Error.WriteLine("Failed to load file 1");
                return;
            }

            if (!Cork.LoadMesh(ID2, "..\\..\\..\\..\\samples\\ballB.off")) {
                Console.Error.WriteLine("Failed to load file 2");
                return;
            }

            Assert(Cork.Union(ID1, ID2, Out1));
            Assert(Cork.SaveMesh(Out1, "Union.stl"));
            Assert(Cork.DeleteMesh(Out1));
            Console.WriteLine("Union done");

            Assert(Cork.Intersection(ID1, ID2, Out1));
            Assert(Cork.SaveMesh(Out1, "Intersection.stl"));
            Assert(Cork.DeleteMesh(Out1));
            Console.WriteLine("Intersection done");

            Assert(Cork.Difference(ID1, ID2, Out1));
            Assert(Cork.SaveMesh(Out1, "Difference.stl"));
            Assert(Cork.DeleteMesh(Out1));
            Console.WriteLine("Difference done");

            Assert(Cork.Xor(ID1, ID2, Out1));
            Assert(Cork.SaveMesh(Out1, "Xor.stl"));
            Console.WriteLine("Xor done");

            Assert(Cork.TranslateZ(Out1, 5.0f));
            Assert(Cork.SaveMesh(Out1, "difference_moved.stl"));
            Console.WriteLine("Move done");

            Assert(Cork.CopyMesh(Out1, Out2));

            Assert(Cork.Rotate180X(Out1));
            Assert(Cork.SaveMesh(Out1, "Difference_moved_and_rotatedX.stl"));
            Console.WriteLine("RotateX done");

            Assert(Cork.Rotate180Y(Out2));
            Assert(Cork.SaveMesh(Out2, "difference_moved_and_rotatedY.stl"));
            Console.WriteLine("RotateY done");
        }
    }
}
