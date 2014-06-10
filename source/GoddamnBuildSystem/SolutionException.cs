using System;

namespace GoddamnEngine.BuildSystem
{
    class SolutionException : Exception
    {
        public SolutionException(string Message)
            : base(Message)
        {
        }
    }
}
