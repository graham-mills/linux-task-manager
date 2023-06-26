import classes from "./ProcessList.module.css";
import Card from "../UI/Card";
import { useCallback, useEffect, useState } from "react";
import { Config } from "../../config/config";
import { ConnectionStatus } from "../../context/conn-status";
import Button from "../UI/Button";
import { Column, FlexTable, Label, Row, Value } from "../UI/FlexTable";

const ProcessList = ({ connStatus }) => {
   const [displayAsTree, setDisplayAsTree] = useState(false);
   const [processes, setProcesses] = useState([]);

   const fetchProcs = useCallback(async () => {
      try {
         const response = await fetch(Config.Endpoints.Procs);
         if (!response.ok) {
            return;
         }
         const data = await response.json();
         setProcesses(data);
      } catch (error) {
         return;
      }
   }, []);

   const pollApi = useCallback(() => {
      if (connStatus === ConnectionStatus.Ok) {
         fetchProcs();
      }
      setTimeout(pollApi, Config.POLL_PERIOD_MS);
   }, [fetchProcs, connStatus]);

   useEffect(() => {
      pollApi();
   }, [pollApi]);

   const tableHeaderRow = (
      <tr>
         <th>PID</th>
         {!displayAsTree && <th>PPID</th>}
         <th>Name</th>
         <th>CPU</th>
         <th>Mem</th>
      </tr>
   );

   const generateProcessRows = (procs) => {
      return procs.map((proc) => (
         <tr key={proc.pid} title={proc.command}>
            <td>
               {displayAsTree && proc.pidPrefix}
               {proc.pid}
            </td>
            {!displayAsTree && <td>{proc.ppid}</td>}
            <td>{proc.name}</td>
            <td>{proc.cpu_usage_percent.toFixed(1)}%</td>
            <td>{proc.mem_usage_percent.toFixed(1)}%</td>
         </tr>
      ));
   };

   const findChildProcs = (parent, nestPrefix) => {
      const children = processes.filter((child) => child.ppid === parent.pid);
      let allDescendants = [];
      children.forEach((child) => {
         child.pidPrefix = nestPrefix + " ";
         allDescendants.push(child);
         allDescendants.push(...findChildProcs(child, child.pidPrefix));
      });
      return allDescendants;
   };

   const generateTreeRows = () => {
      // Start by finding root proc
      const rootProcs = processes.filter((proc) => proc.ppid === 0);
      if (rootProcs.length === 0)
      {
        return "";
      }
      const allProcs = [rootProcs[0], ...findChildProcs(rootProcs[0], " ")];
      return generateProcessRows(allProcs);
   };

   let tableRows = "";
   if (displayAsTree) {
      tableRows = generateTreeRows();
   } else {
      tableRows = generateProcessRows(processes);
   }

   return (
      <Card title="Processes">
         <FlexTable columns={2}>
            <Column>
               <Row>
                  <Label>Processes</Label>
                  <Value>{processes.length}</Value>
               </Row>
            </Column>
            <Column>
               <Row>
                  <Value>
                     <Button
                        selected={!displayAsTree}
                        onClick={() => {
                           setDisplayAsTree(false);
                        }}
                     >
                        List View
                     </Button>
                     <Button
                        selected={displayAsTree}
                        onClick={() => {
                           setDisplayAsTree(true);
                        }}
                     >
                        Tree View
                     </Button>
                  </Value>
               </Row>
            </Column>
         </FlexTable>

         <table className={classes["proc-table"]}>
            <thead>{tableHeaderRow}</thead>
            <tbody>{tableRows}</tbody>
         </table>
      </Card>
   );
};

export default ProcessList;
