using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Deployment.WindowsInstaller;
using System.Diagnostics;
namespace KKS.IsKeyboardRkt.Setup.CustomAction
{
    public class CustomActions
    {
        [CustomAction]
        public static ActionResult ExecuteInstallInf(Session session)
        {            
            // Get Path To Application
            string customActionData = session["CustomActionData"];
			
            // Register KKS.KmdIsKeyboardRkt.inf
            ProcessStartInfo startInfoKKRkt = new ProcessStartInfo();            
            startInfoKKRkt.WorkingDirectory = customActionData;
			startInfoKKRkt.Verb = "Open";// install";
			startInfoKKRkt.FileName = "devcon";
			startInfoKKRkt.Arguments = @"install ""KKS.KmdIsKeyboardRkt.inf"" ""Root\IsKeyboardRootKitted""";
			using (Process exeProcess = System.Diagnostics.Process.Start(startInfoKKRkt))
            {
                exeProcess.WaitForExit();
            }

            return ActionResult.Success;
        }

		[CustomAction]
		public static ActionResult ExecuteUnInstallInf(Session session)
		{
			// Get Path To Application
			string customActionData = session["CustomActionData"];

			// Register KKS.KmdIsKeyboardRkt.inf
			ProcessStartInfo startInfoKKRkt = new ProcessStartInfo();
			startInfoKKRkt.WorkingDirectory = customActionData;
			startInfoKKRkt.Verb = "Open";// install";
			startInfoKKRkt.FileName = "devcon";
			startInfoKKRkt.Arguments = @"remove ""KKS.KmdIsKeyboardRkt.inf"" ""Root\IsKeyboardRootKitted""";
			using (Process exeProcess = System.Diagnostics.Process.Start(startInfoKKRkt))
			{
				exeProcess.WaitForExit();
			}

			return ActionResult.Success;
		}
	}
}
