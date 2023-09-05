import classes from "./ProcessList.module.css";

// This component displays processes in a table in their hierarchical order -
// rows for child processes are grouped under the row of their parent process,
// determined by their PIDs and PPIDs.
const TreeTable = ({ processes }) => {
   let tableRows = <EmptyRow />;
   if (processes.length > 0) {
      const rootProc = processes.filter((proc) => proc.ppid === 0)[0];
      const allProcs = [
         rootProc,
         ...sortProcsDepthFirst(rootProc, processes, " "),
      ];
      tableRows = allProcs.map((proc) => (
         <TableProcessRow key={proc.pid} proc={proc} />
      ));
   }

   return (
      <table className={classes["proc-table"]}>
         <thead>
            <TableHeaderRow />
         </thead>
         <tbody>{tableRows}</tbody>
      </table>
   );
};
export default TreeTable;

// <tr> element displaying process information
const TableProcessRow = ({ proc }) => (
   <tr title={proc.command}>
      <td>
         {proc.pidPrefix}
         {proc.pid}
      </td>
      <td>{proc.name}</td>
      <td>{proc.cpu_usage_percent.toFixed(1)}%</td>
      <td>{proc.mem_usage_percent.toFixed(1)}%</td>
   </tr>
);

// Empty/placeholder <tr> element to be displayed
// when the table has no data to display
const EmptyRow = () => (
   <tr>
      <td />
      <td />
      <td />
      <td />
   </tr>
);

const TableHeaderRow = () => (
   <tr>
      <th>PID</th>
      <th>Name</th>
      <th>CPU</th>
      <th>Mem</th>
   </tr>
);

/*
Returns a sorted list of all descendant processes of a parent process
as a flattened tree structure.
Example:
   Inputs:
      parent = {PID: 1}
      allProcs = [
                   {PID: 1, PPID: 0},
                   {PID: 2, PPID: 1},
                   {PID: 3, PPID: 1},
                   {PID: 4, PPID: 2} <-- Child of PID 2
                 ]
   Output:
      [
         {PID: 2, ...},
         {PID: 4, ...}, <-- Child of PID 2 moved
         {PID: 3, ...}
      ]
*/
const sortProcsDepthFirst = (parent, allProcs, nestPrefix) => {
   const children = allProcs.filter((child) => child.ppid === parent.pid);
   let allDescendants = [];
   children.forEach((child) => {
      child.pidPrefix = nestPrefix + " ";
      allDescendants.push(child);
      allDescendants.push(
         ...sortProcsDepthFirst(child, allProcs, child.pidPrefix)
      );
   });
   return allDescendants;
};
