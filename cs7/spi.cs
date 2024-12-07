using System;
using System.Runtime.InteropServices;

namespace SPI
{
    public class spi
    {
        /* Error handling functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern string spi_Error_get();

        public delegate void IntPtr_delete(IntPtr v);

        // The point of PointerHandle is to contain a converted value which is
        // represented as IntPtr and to manage the memory associated with the IntPtr
        //
        // Inputs to a function in C# are converted and may well create IntPtr
        // We put the IntPtr inside a PointerHandle and then pass get_inner() to
        // the C function
        //
        // Outputs of type IntPtr from a function in C should be immediately wrapped
        // inside PointerHandle before the C to C# conversion is performed.
        // This makes it easy to dispose of the IntPtr on exiting the function.

        public class PointerHandle : IDisposable
        {
            public PointerHandle(IntPtr in_ptr, IntPtr_delete in_deleter) 
            {
                ptr = in_ptr;
                deleter = in_deleter;
            }

            public IntPtr get_inner()
            {
                return ptr;
            }

            private IntPtr ptr;
            private IntPtr_delete deleter;

            ~PointerHandle()
            {
                Dispose(false);
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            protected void Dispose(bool dispose)
            {
                if (ptr != IntPtr.Zero)
                {
                    deleter(ptr);
                    ptr = IntPtr.Zero;
                }
            }
        }

        public static Exception ErrorToException()
        {
            System.String error = spi_Error_get();
            return new Exception(error);
        }

        /* Date functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_from_YMD(
            int year, int month, int day, out int date);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_YMD(
            int date, out int year, out int month, out int day);

        public static System.DateTime DateFromCDate(int dt)
        {
            if (dt == 0)
                return default(System.DateTime);

            if (spi_Date_YMD(dt, out int year, out int month, out int day) != 0)
            {
                throw ErrorToException();
            }
            return new System.DateTime(year, month, day);
        }
        public static int DateToCDate(System.DateTime dt)
        { 
            if (dt == default(System.DateTime))
                return 0;

            if (spi_Date_from_YMD(dt.Year, dt.Month, dt.Day, out int result) != 0)
            {
                throw ErrorToException();
            }
            return result;
        }

        /* DateTime functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_from_YMDHMS(
            int year, int month, int day, int hours, int minutes, int seconds,
            out double dateTime);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_YMDHMS(
            double date,
            out int year, out int month, out int day,
            out int hours, out int minutes, out int seconds);

        public static System.DateTime DateTimeFromCDateTime(double dt)
        {
            if (dt == 0.0)
                return default(System.DateTime);

            if (spi_DateTime_YMDHMS(dt, out int year, out int month, out int day,
                                    out int hours, out int minutes, out int seconds) != 0)
            {
                throw ErrorToException();
            }
            return new System.DateTime(year, month, day, hours, minutes, seconds);
        }
        public static double DateTimeToCDateTime(System.DateTime dt)
        {
            if (dt == default(System.DateTime))
                return 0.0;

            if (spi_DateTime_from_YMDHMS(
                dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second,
                out double result) != 0)
            {
                throw ErrorToException();
            }
            return result;
        }

        ///* Object functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Object_delete(IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_get_object_id(IntPtr item, out string objectId);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_get_class_name(IntPtr item, out string className);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_to_string(
            IntPtr self,
            string format,
            string options,
            out string str);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Object_from_string(
            string str);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_to_file(
            IntPtr self,
            string filename,
            string format,
            string options);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Object_from_file(
            string filename);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Object_get_value(
            IntPtr self,
            string name);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_save(
            string baseName,
            IntPtr obj,
            bool noCount,
            out string handle);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_find(
            string handle,
            out IntPtr obj);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_free_all(
            out int count);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_count(
            string className,
            out int count);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_free(
            string handle);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_list(
            string baseName,
            string className,
            out IntPtr handles);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_handle_class_name(
            string handle,
            out string className);

        /* Variant functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Variant_delete(IntPtr self);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_new_String(string str);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_new_Date(int dt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_new_DateTime(double dt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_new_Double(double d);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_new_Bool(bool b);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_new_Int(int i);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_String(
            IntPtr var,
            out string str);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Date(
            IntPtr var,
            out int dt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_DateTime(
            IntPtr var,
            out double dt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Double(
            IntPtr var,
            out double d);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Bool(
            IntPtr var,
            out bool b);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Int(
            IntPtr var,
            out int i);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Object(
            IntPtr var,
            out IntPtr obj);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_String_Vector(
            IntPtr var,
            out IntPtr str);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Date_Vector(
            IntPtr var,
            out IntPtr dt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_DateTime_Vector(
            IntPtr var,
            out IntPtr dt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Double_Vector(
            IntPtr var,
            out IntPtr d);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Bool_Vector(
            IntPtr var,
            out IntPtr b);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Int_Vector(
            IntPtr var,
            out IntPtr i);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Object_Vector(
            IntPtr var,
            out IntPtr obj);


        /* vector functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Int_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Int_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Int_Vector_item(
            IntPtr v,
            int i,
            out int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Int_Vector_set_item(
            IntPtr v,
            int i,
            int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Int_Vector_size(
            IntPtr v,
            out int size);

        public static PointerHandle IntVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Int_Vector_delete);
        }

        public static int[] IntVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Int_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            int[] array = new int[size];

            for (int i = 0; i < size; ++i)
            {
                if (spi_Int_Vector_item(v, i, out array[i]) != 0)
                {
                    throw ErrorToException();
                }
            }

            return array;
        }

        public static PointerHandle IntVectorFromArray(int[] array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_Int_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Int_Vector_delete);

            for (int i = 0; i < size; ++i)
            {
                if (spi_Int_Vector_set_item(v, i, array[i]) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Double_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Double_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Double_Vector_item(
            IntPtr v,
            int i,
            out double item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Double_Vector_set_item(
            IntPtr v,
            int i,
            double item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Double_Vector_size(
            IntPtr v,
            out int size);

        public static PointerHandle DoubleVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Double_Vector_delete);
        }

        public static double[] DoubleVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Double_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            double[] array = new double[size];

            for (int i = 0; i < size; ++i)
            {
                if (spi_Double_Vector_item(v, i, out array[i]) != 0)
                {
                    throw ErrorToException();
                }
            }

            return array;
        }

        public static PointerHandle DoubleVectorFromArray(double[] array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_Double_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Double_Vector_delete);

            for (int i = 0; i < size; ++i)
            {
                if (spi_Double_Vector_set_item(v, i, array[i]) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Bool_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Bool_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Bool_Vector_item(
            IntPtr v,
            int i,
            out bool item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Bool_Vector_set_item(
            IntPtr v,
            int i,
            bool item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Bool_Vector_size(
            IntPtr v,
            out int size);

        public static PointerHandle BoolVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Bool_Vector_delete);
        }

        public static bool[] BoolVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Bool_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            bool[] array = new bool[size];

            for (int i = 0; i < size; ++i)
            {
                if (spi_Bool_Vector_item(v, i, out array[i]) != 0)
                {
                    throw ErrorToException();
                }
            }

            return array;
        }

        public static PointerHandle BoolVectorFromArray(bool[] array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_Bool_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Bool_Vector_delete);

            for (int i = 0; i < size; ++i)
            {
                if (spi_Bool_Vector_set_item(v, i, array[i]) != 0)
                {
                    spi_Bool_Vector_delete(v);
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Date_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Date_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_Vector_item(
            IntPtr v,
            int i,
            out int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_Vector_set_item(
            IntPtr v,
            int i,
            int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_Vector_size(
            IntPtr v,
            out int size);

        public static PointerHandle DateVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Date_Vector_delete);
        }

        public static System.DateTime[] DateVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Date_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            System.DateTime[] array = new System.DateTime[size];

            for (int i = 0; i < size; ++i)
            {
                if (spi_Date_Vector_item(v, i, out int cdate) != 0)
                {
                    throw ErrorToException();
                }
                array[i] = DateFromCDate(cdate);
            }

            return array;
        }

        public static PointerHandle DateVectorFromArray(System.DateTime[] array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_Date_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }

            PointerHandle h = new PointerHandle(v, spi_Date_Vector_delete);
            for (int i = 0; i < size; ++i)
            {
                int cdate = DateToCDate(array[i]);
                if (spi_Date_Vector_set_item(v, i, cdate) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_DateTime_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_DateTime_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_Vector_item(
            IntPtr v,
            int i,
            out double item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_Vector_set_item(
            IntPtr v,
            int i,
            double item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_Vector_size(
            IntPtr v,
            out int size);


        public static PointerHandle DateTimeVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_DateTime_Vector_delete);
        }

        public static System.DateTime[] DateTimeVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_DateTime_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            System.DateTime[] array = new System.DateTime[size];

            for (int i = 0; i < size; ++i)
            {
                if (spi_DateTime_Vector_item(v, i, out double cdate) != 0)
                {
                    throw ErrorToException();
                }
                array[i] = DateTimeFromCDateTime(cdate);
            }

            return array;
        }

        public static PointerHandle DateTimeVectorFromArray(System.DateTime[] array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_DateTime_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }

            PointerHandle h = new PointerHandle(v, spi_DateTime_Vector_delete);
            for (int i = 0; i < size; ++i)
            {
                double cdate = DateTimeToCDateTime(array[i]);
                if (spi_DateTime_Vector_set_item(v, i, cdate) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_String_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_String_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_String_Vector_item(
            IntPtr v,
            int i,
            out string item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_String_Vector_set_item(
            IntPtr v,
            int i,
            string item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_String_Vector_size(
            IntPtr v,
            out int size);

        public static PointerHandle StringVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_String_Vector_delete);
        }

        public static string[] StringVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_String_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            string[] array = new string[size];

            for (int i = 0; i < size; ++i)
            {
                if (spi_String_Vector_item(v, i, out array[i]) != 0)
                {
                    throw ErrorToException();
                }
            }

            return array;
        }

        public static PointerHandle StringVectorFromArray(string[] array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_String_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_String_Vector_delete);

            for (int i = 0; i < size; ++i)
            {
                if (spi_String_Vector_set_item(v, i, array[i]) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Object_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Object_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_Vector_item(
            IntPtr v,
            int i,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_Vector_set_item(
            IntPtr v,
            int i,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_Vector_size(
            IntPtr v,
            out int size);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Instance_Vector_item(
            IntPtr v,
            int i,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Instance_Vector_set_item(
            IntPtr v,
            int i,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Instance_Vector_size(
            IntPtr v,
            out int size);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Enum_Vector_item(
            IntPtr v,
            int i,
            out int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Enum_Vector_set_item(
            IntPtr v,
            int i,
            int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Enum_Vector_size(
            IntPtr v,
            out int size);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Variant_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Vector_item(
            IntPtr v,
            int i,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Vector_set_item(
            IntPtr v,
            int i,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Vector_size(
            IntPtr v,
            out int size);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Map_Vector_delete(IntPtr v);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Map_Vector_new(
            int N);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Map_Vector_item(
            IntPtr v,
            int i,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Map_Vector_set_item(
            IntPtr v,
            int i,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Map_Vector_size(
            IntPtr v,
            out int size);

        /* matrix functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Int_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Int_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Int_Matrix_item(
            IntPtr m,
            int r, int c,
            out int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Int_Matrix_set_item(
            IntPtr m,
            int r, int c,
            int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Int_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        public static PointerHandle IntMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Int_Matrix_delete);
        }

        public static int[,] IntMatrixToArray(PointerHandle h)
        {
            IntPtr m = h.get_inner();
            int nr, nc;
            if (spi_Int_Matrix_size(m, out nr, out nc) != 0)
            {
                throw ErrorToException();
            }

            int[,] array = new int[nr, nc];

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Int_Matrix_item(m, i, j, out array[i, j]) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }
            return array;
        }

        public static PointerHandle IntMatrixFromArray(int[,] array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr m = spi_Int_Matrix_new(nr, nc);
            if (m == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(m, spi_Int_Matrix_delete);

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Int_Matrix_set_item(m, i, j, array[i, j]) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Double_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Double_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Double_Matrix_item(
            IntPtr m,
            int r, int c,
            out double item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Double_Matrix_set_item(
            IntPtr m,
            int r, int c,
            double item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Double_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        public static PointerHandle DoubleMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Double_Matrix_delete);
        }

        public static double[,] DoubleMatrixToArray(PointerHandle h)
        {
            IntPtr m = h.get_inner();
            int nr, nc;
            if (spi_Double_Matrix_size(m, out nr, out nc) != 0)
            {
                throw ErrorToException();
            }

            double[,] array = new double[nr, nc];

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Double_Matrix_item(m, i, j, out array[i, j]) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }
            return array;
        }

        public static PointerHandle DoubleMatrixFromArray(double[,] array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr m = spi_Double_Matrix_new(nr, nc);
            if (m == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(m, spi_Double_Matrix_delete);

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Double_Matrix_set_item(m, i, j, array[i, j]) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Bool_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Bool_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Bool_Matrix_item(
            IntPtr m,
            int r, int c,
            out int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Bool_Matrix_set_item(
            IntPtr m,
            int r, int c,
            int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Bool_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        public static PointerHandle BoolMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Bool_Matrix_delete);
        }

        public static bool[,] BoolMatrixToArray(PointerHandle h)
        {
            IntPtr m = h.get_inner();
            int nr, nc;
            if (spi_Bool_Matrix_size(m, out nr, out nc) != 0)
            {
                throw ErrorToException();
            }

            bool[,] array = new bool[nr, nc];

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Bool_Matrix_item(m, i, j, out int b) != 0)
                    {
                        throw ErrorToException();
                    }
                    array[i, j] = b != 0;
                }
            }
            return array;
        }

        public static PointerHandle BoolMatrixFromArray(bool[,] array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr m = spi_Bool_Matrix_new(nr, nc);
            if (m == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(m, spi_Bool_Matrix_delete);

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Bool_Matrix_set_item(m, i, j, array[i, j] ? 1 : 0) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Date_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Date_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_Matrix_item(
            IntPtr m,
            int r, int c,
            out int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_Matrix_set_item(
            IntPtr m,
            int r, int c,
            int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Date_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);


        public static PointerHandle DateMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Date_Matrix_delete);
        }

        public static System.DateTime[,] DateMatrixToArray(PointerHandle h)
        {
            IntPtr m = h.get_inner();
            int nr, nc;
            if (spi_Date_Matrix_size(m, out nr, out nc) != 0)
            {
                throw ErrorToException();
            }

            System.DateTime[,] array = new System.DateTime[nr, nc];

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Date_Matrix_item(m, i, j, out int cdt) != 0)
                    {
                        throw ErrorToException();
                    }
                    array[i, j] = spi.DateFromCDate(cdt);
                }
            }
            return array;
        }

        public static PointerHandle DateMatrixFromArray(System.DateTime[,] array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr m = spi_Date_Matrix_new(nr, nc);
            if (m == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(m, spi_Date_Matrix_delete);

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Date_Matrix_set_item(m, i, j, spi.DateToCDate(array[i, j])) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_DateTime_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_DateTime_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_Matrix_item(
            IntPtr m,
            int r, int c,
            out double item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_Matrix_set_item(
            IntPtr m,
            int r, int c,
            double item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_DateTime_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        public static PointerHandle DateTimeMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_DateTime_Matrix_delete);
        }

        public static System.DateTime[,] DateTimeMatrixToArray(PointerHandle h)
        {
            IntPtr m = h.get_inner();
            int nr, nc;
            if (spi_DateTime_Matrix_size(m, out nr, out nc) != 0)
            {
                throw ErrorToException();
            }

            System.DateTime[,] array = new System.DateTime[nr, nc];

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_DateTime_Matrix_item(m, i, j, out double cdt) != 0)
                    {
                        throw ErrorToException();
                    }
                    array[i, j] = spi.DateTimeFromCDateTime(cdt);
                }
            }
            return array;
        }

        public static PointerHandle DateTimeMatrixFromArray(System.DateTime[,] array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr m = spi_DateTime_Matrix_new(nr, nc);
            if (m == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(m, spi_DateTime_Matrix_delete);

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_DateTime_Matrix_set_item(m, i, j, spi.DateTimeToCDateTime(array[i, j])) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_String_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_String_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_String_Matrix_item(
            IntPtr m,
            int r, int c,
            out string item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_String_Matrix_set_item(
            IntPtr m,
            int r, int c,
            string item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_String_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        public static PointerHandle StringMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_String_Matrix_delete);
        }

        public static string[,] StringMatrixToArray(PointerHandle h)
        {
            IntPtr m = h.get_inner();
            int nr, nc;
            if (spi_String_Matrix_size(m, out nr, out nc) != 0)
            {
                throw ErrorToException();
            }

            string[,] array = new string[nr, nc];

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_String_Matrix_item(m, i, j, out array[i, j]) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }
            return array;
        }

        public static PointerHandle StringMatrixFromArray(string[,] array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr m = spi_String_Matrix_new(nr, nc);
            if (m == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(m, spi_String_Matrix_delete);

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_String_Matrix_set_item(m, i, j, array[i, j]) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }

            return h;
        }

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Object_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Object_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_Matrix_item(
            IntPtr m,
            int r, int c,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_Matrix_set_item(
            IntPtr m,
            int r, int c,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Object_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Instance_Matrix_item(
             IntPtr m,
            int r, int c,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Instance_Matrix_set_item(
            IntPtr m,
            int r, int c,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Instance_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);


        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Enum_Matrix_item(
             IntPtr m,
            int r, int c,
            out int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Enum_Matrix_set_item(
            IntPtr m,
            int r, int c,
            int item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Enum_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);


        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_type(IntPtr m, out string vt);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Variant_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Variant_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Matrix_item(
            IntPtr m,
            int r, int c,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Matrix_set_item(
            IntPtr m,
            int r, int c,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Variant_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern void spi_Map_Matrix_delete(IntPtr m);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Map_Matrix_new(
            int nr, int nc);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Map_Matrix_item(
            IntPtr m,
            int r, int c,
            out IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Map_Matrix_set_item(
            IntPtr m,
            int r, int c,
            IntPtr item);

        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern int spi_Map_Matrix_size(
            IntPtr m,
            out int nr,
            out int nc);

        public class Variant : IDisposable
        {
            public Variant(string str)
            {
                self = spi_Variant_new_String(str);
                if (self == IntPtr.Zero)
                    throw ErrorToException();
            }

            public Variant(System.DateTime dt)
            {
                self = spi_Variant_new_DateTime(DateTimeToCDateTime(dt));
                if (self == IntPtr.Zero)
                    throw ErrorToException();
            }

            public Variant(double d)
            {
                self = spi_Variant_new_Double(d);
                if (self == IntPtr.Zero)
                    throw ErrorToException();
            }

            public Variant(bool b)
            {
                self = spi_Variant_new_Bool(b);
                if (self == IntPtr.Zero)
                    throw ErrorToException();
            }

            public Variant(int i)
            {
                self = spi_Variant_new_Int(i);
                if (self == IntPtr.Zero)
                    throw ErrorToException();
            }

            ~Variant()
            {
                Dispose(false);
            }

            protected virtual void Dispose(bool disposing)
            {
                if (self != IntPtr.Zero)
                {
                    spi_Variant_delete(self);
                    self = IntPtr.Zero;
                }
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            public Variant(IntPtr in_self)
            {
                self = in_self;
            }

            public static IntPtr get_inner(Variant v)
            {
                if (v == null)
                    return IntPtr.Zero;

                return v.self;
            }

            public string type()
            {
                if (spi_Variant_type(self, out string vt) != 0)
                {
                    throw spi.ErrorToException();
                }
                return vt;
            }

            static public implicit operator System.DateTime(Variant var)
            {
                if (spi_Variant_DateTime(var.self, out double dt) != 0)
                    throw ErrorToException();

                return DateTimeFromCDateTime(dt);
            }

            static public implicit operator System.String(Variant var)
            {
                if (spi_Variant_String(var.self, out string str) != 0)
                    throw ErrorToException();

                return str;
            }

            static public implicit operator double(Variant var)
            {
                if (spi_Variant_Double(var.self, out double d) != 0)
                    throw ErrorToException();

                return d;
            }

            static public implicit operator int(Variant var)
            {
                if (spi_Variant_Int(var.self, out int i) != 0)
                    throw ErrorToException();

                return i;
            }

            static public implicit operator bool(Variant var)
            {
                if (spi_Variant_Bool(var.self, out bool b) != 0)
                    throw ErrorToException();

                return b;
            }

            static public implicit operator spi.Object(Variant var)
            {
                if (spi_Variant_Object(var.self, out IntPtr o) != 0)
                    throw ErrorToException();

                return spi.Object.Wrap(o);
            }

            static public implicit operator System.DateTime[](Variant var)
            {
                if (spi_Variant_DateTime_Vector(var.self, out IntPtr dt) != 0)
                    throw ErrorToException();

                PointerHandle h = spi.DateTimeVectorToHandle(dt);
                return spi.DateTimeVectorToArray(h);
            }

            static public implicit operator System.String[](Variant var)
            {
                if (spi_Variant_String_Vector(var.self, out IntPtr dt) != 0)
                    throw ErrorToException();

                PointerHandle h = spi.StringVectorToHandle(dt);
                return spi.StringVectorToArray(h);
            }

            static public implicit operator double[](Variant var)
            {
                if (spi_Variant_Double_Vector(var.self, out IntPtr dt) != 0)
                    throw ErrorToException();

                PointerHandle h = spi.DoubleVectorToHandle(dt);
                return spi.DoubleVectorToArray(h);
            }

            static public implicit operator int[](Variant var)
            {
                if (spi_Variant_Int_Vector(var.self, out IntPtr dt) != 0)
                    throw ErrorToException();

                PointerHandle h = spi.IntVectorToHandle(dt);
                return spi.IntVectorToArray(h);
            }

            static public implicit operator bool[](Variant var)
            {
                if (spi_Variant_Bool_Vector(var.self, out IntPtr dt) != 0)
                    throw ErrorToException();

                PointerHandle h = spi.BoolVectorToHandle(dt);
                return spi.BoolVectorToArray(h);
            }

            static public implicit operator spi.Object[](Variant var)
            {
                if (spi_Variant_Object_Vector(var.self, out IntPtr dt) != 0)
                    throw ErrorToException();

                PointerHandle h = spi.ObjectVectorToHandle(dt);
                return spi.ObjectVectorToArray(h);
            }

            // note that self = spi::Variant*
            private IntPtr self = IntPtr.Zero;
        }

        public static PointerHandle VariantVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Variant_Vector_delete);
        }

        public static Variant[] VariantVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Variant_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            Variant[] array = new Variant[size];

            for (int i = 0; i < size; ++i)
            {
                if (spi_Variant_Vector_item(v, i, out IntPtr item) != 0)
                {
                    throw ErrorToException();
                }
                array[i] = new Variant(item);
            }

            return array;
        }

        public static PointerHandle VariantVectorFromArray(Variant[] array)
        {
            int size = array is null ? 0 : array.Length;
            IntPtr v = spi_Variant_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Variant_Vector_delete);

            for (int i = 0; i < size; ++i)
            {
                if (spi_Variant_Vector_set_item(v, i, spi.Variant.get_inner(array[i])) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        public static PointerHandle VariantMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Variant_Matrix_delete);
        }

        public static Variant[,] VariantMatrixToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            if (spi_Variant_Matrix_size(v, out int nr, out int nc) != 0)
            {
                throw ErrorToException();
            }

            Variant[,] array = new Variant[nr, nc];

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Variant_Matrix_item(v, i, j, out IntPtr item) != 0)
                    {
                        throw ErrorToException();
                    }
                    array[i,j] = new Variant(item);
                }
            }

            return array;
        }

        public static PointerHandle VariantMatrixFromArray(Variant[,] array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr v = spi_Variant_Matrix_new(nr,nc);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Variant_Matrix_delete);

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Variant_Matrix_set_item(v, i, j, spi.Variant.get_inner(array[i,j])) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }

            return h;
        }

        // TBA: should we allow these classes which wrap a reference counted pointer
        // to have one constructor which takes ownership and another which increments
        // the reference count?

        public class Object : IDisposable
        {
            protected Object()
            {}

            ~Object()
            {
                Dispose(false);
            }

            protected virtual void Dispose(bool disposing)
            {
                if (self != IntPtr.Zero)
                {
                    spi_Object_delete(self);
                    self = IntPtr.Zero;
                }
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            public delegate spi.Object class_wrapper(IntPtr self);
            public static System.Collections.Generic.Dictionary<string, class_wrapper> zz_class_wrappers;

            static Object()
            {
                zz_class_wrappers = new System.Collections.Generic.Dictionary<string, class_wrapper>();
                zz_class_wrappers.Add("Map", spi.Map.Wrap);
            }

            public static void zz_register_class_wrapper(string name, class_wrapper wrapper)
            {
                zz_class_wrappers.Add(name, wrapper);
            }

            public static spi.Object Wrap(IntPtr self)
            {
                if (self == IntPtr.Zero)
                    return null;

                if (spi_Object_get_class_name(self, out string className) != 0)
                    throw spi.ErrorToException();

                try
                {
                    class_wrapper wrapper = zz_class_wrappers[className];
                    return wrapper(self);
                }
                catch
                {
                    spi.Object obj = new spi.Object();
                    obj.set_inner(self);
                    return obj;
                }
            }

            public string object_id
            {
                get
                {
                    if (self == IntPtr.Zero)
                        return null;

                    if (spi_Object_get_object_id(self, out string objectId) != 0)
                        throw spi.ErrorToException();

                    return objectId;
                }
            }

            public string typename
            {
                get
                {
                    if (self == IntPtr.Zero)
                        return null;

                    if (spi_Object_get_class_name(self, out string typename) != 0)
                        throw spi.ErrorToException();

                    return typename;
                }
            }

            /// <summary>
            /// Serializes the object to string using the given format
            /// </summary>
            /// <param name="format">
            /// Optional. Object string format.
            /// </param>
            /// <param name="options">
            /// Optional. Extra options for the given format.
            /// </param>
            /// <returns></returns>
            public System.String to_string(System.String format = "", System.String options = "")
            {
                if (spi_Object_to_string(self, format, options, out string str) != 0)
                {
                    throw ErrorToException();
                }
                return str;
            }

            /// <summary>
            /// De-serializes the string to an spi.Object
            /// </summary>
            /// <param name="str">
            /// The serialized object string, as created by the to_string method.
            /// </param>
            /// <returns></returns>
            public static spi.Object from_string(System.String str)
            {
                IntPtr inner = spi_Object_from_string(str);
                if (inner == IntPtr.Zero)
                    throw spi.ErrorToException();
                return spi.Object.Wrap(inner);
            }

            /// <summary>
            /// Serializes the object to file using the given format.
            /// </summary>
            /// <param name="filename">
            /// Name of the file to be written.
            /// </param>
            /// <param name="format">
            /// Optional. Object file format.
            /// </param>
            /// <param name="options">
            /// Optional. extra options for the given format.
            /// </param>
            /// <returns></returns>
            public System.String to_file(System.String filename,
                System.String format = "", System.String options = "")
            {
                if (spi_Object_to_file(self, filename, format, options) != 0)
                {
                    throw ErrorToException();
                }
                return filename;
            }

            /// <summary>
            /// De-serializes the object stored on file to an spi.Object
            /// </summary>
            /// <param name="filename">
            /// The filename containing the serialized object, e.g. as created by the to_file method.
            /// </param>
            /// <returns></returns>
            public static spi.Object from_file(System.String filename)
            {
                IntPtr inner = spi_Object_from_file(filename);
                if (inner == IntPtr.Zero)
                    throw spi.ErrorToException();
                return spi.Object.Wrap(inner);
            }

            /// <summary>
            /// Equivalent to object_get in Excel.
            /// </summary>
            /// <param name="name"></param>
            /// <returns>
            /// Variant value - you can then use implicit type conversions to
            /// convert to the actual type, e.g. int numItems = o.get_value("numItems")
            /// </returns>
            public Variant get_value(System.String name)
            {
                IntPtr value = spi_Object_get_value(self, name);
                if (value == IntPtr.Zero)
                    throw ErrorToException();

                return new Variant(value);
            }

            protected void set_inner(IntPtr self)
            {
                // in_self should have been returned via P/INVOKE with reference
                // count incremented by one
                //
                // hence if it is the same as self then we can safely delete self
                //
                // note that the only time we would change self for an existing
                // object is after we have changed one of its attributes
                if (self == IntPtr.Zero)
                    throw new Exception("Cannot set_inner pointer to NULL");

                if (this.self != IntPtr.Zero)
                    spi_Object_delete(this.self);

                this.self = self;
            }

            public static IntPtr get_inner(spi.Object o)
            {
                if (o == null)
                    return IntPtr.Zero;

                return o.self;
            }

            // note that self = spi::Object* or sub-class thereof
            protected IntPtr self = IntPtr.Zero;
        }

        // functions dealing with string object handles
        public static System.String ObjectHandleSave(
            System.String baseName,
            spi.Object obj,
            bool noCount = false)
        {
            if (spi_Object_handle_save(baseName, spi.Object.get_inner(obj), noCount, out string handle) != 0)
            {
                throw ErrorToException();
            }
            return handle;
        }

        public static spi.Object ObjectHandleFind(
            System.String handle)
        {
            if (spi_Object_handle_find(handle, out IntPtr obj) != 0)
            {
                throw ErrorToException();
            }
            return spi.Object.Wrap(obj);
        }

        public static int ObjectHandleFreeAll()
        {
            if (spi_Object_handle_free_all(out int count) != 0)
            {
                throw ErrorToException();
            }
            return count;
        }

        public static int ObjectHandleCount(
            System.String className)
        {
            if (spi_Object_handle_count(className, out int count) != 0)
            {
                throw ErrorToException();
            }
            return count;
        }

        public static void ObjectHandleFree(
            System.String handle)
        {
            if (spi_Object_handle_free(handle) != 0)
            {
                throw ErrorToException();
            }
        }

        public static string[] ObjectHandleList(
            System.String baseName = "",
            System.String className = "")
        {
            if (spi_Object_handle_list(baseName, className, out IntPtr handles) != 0)
            {
                throw ErrorToException();
            }
            return spi.StringVectorToArray(spi.StringVectorToHandle(handles));
        }

        public static string ObjectHandleClassName(
            string handle)
        {
            if (spi_Object_handle_class_name(handle, out string className) != 0)
            {
                throw ErrorToException();
            }
            return className;
        }

        public static PointerHandle ObjectVectorToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Object_Vector_delete);
        }

        public static spi.Object[] ObjectVectorToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            int size;
            if (spi_Object_Vector_size(v, out size) != 0)
            {
                throw ErrorToException();
            }

            spi.Object[] array = new spi.Object[size];

            for (int i = 0; i < size; ++i)
            {
                if (spi_Object_Vector_item(v, i, out IntPtr item) != 0)
                {
                    throw ErrorToException();
                }
                array[i] = spi.Object.Wrap(item);
            }

            return array;
        }

        public static PointerHandle ObjectVectorFromArray(spi.Object[] array)
        {
            int size = array.Length;
            IntPtr v = spi_Object_Vector_new(size);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Object_Vector_delete);

            for (int i = 0; i < size; ++i)
            {
                if (spi_Object_Vector_set_item(v, i, spi.Object.get_inner(array[i])) != 0)
                {
                    throw ErrorToException();
                }
            }

            return h;
        }

        public static PointerHandle ObjectMatrixToHandle(IntPtr v)
        {
            return new PointerHandle(v, spi_Object_Matrix_delete);
        }

        public static spi.Object[,] ObjectMatrixToArray(PointerHandle h)
        {
            IntPtr v = h.get_inner();
            if (spi_Object_Matrix_size(v, out int nr, out int nc) != 0)
            {
                throw ErrorToException();
            }

            spi.Object[,] array = new spi.Object[nr, nc];

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Object_Matrix_item(v, i, j, out IntPtr item) != 0)
                    {
                        throw ErrorToException();
                    }
                    array[i, j] = spi.Object.Wrap(item);
                }
            }

            return array;
        }

        public static PointerHandle ObjectMatrixFromArray(spi.Object[,] array)
        {
            int nr = array is null ? 0 : array.GetLength(0);
            int nc = array is null ? 0 : array.GetLength(1);

            IntPtr v = spi_Object_Matrix_new(nr, nc);
            if (v == IntPtr.Zero)
            {
                throw ErrorToException();
            }
            PointerHandle h = new PointerHandle(v, spi_Object_Matrix_delete);

            for (int i = 0; i < nr; ++i)
            {
                for (int j = 0; j < nc; ++j)
                {
                    if (spi_Object_Matrix_set_item(v, i, j, spi.Object.get_inner(array[i, j])) != 0)
                    {
                        throw ErrorToException();
                    }
                }
            }

            return h;
        }

        /* Map functions */
        [DllImport("spi-c", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr spi_Map_dynamic_cast(IntPtr o);

        public class Map : spi.Object
        {
            protected Map() { }

            // self should be spi::MapObject*
            public new static Map Wrap(IntPtr self)
            {
                if (self == IntPtr.Zero)
                    return null;

                self = spi_Map_dynamic_cast(self);
                if (self == IntPtr.Zero)
                    throw new Exception("self is not an instance of Map");

                Map obj = new Map();
                obj.set_inner(self);
                return obj;
            }

        }
    }
}
