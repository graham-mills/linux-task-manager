import classes from "./ProcessList.module.css";

const EmptyRow = () => (
   <>
      <tr>
         <td />
         <td />
         <td />
         <td />
      </tr>
   </>
);

const TreeTable = ({ processes }) => {
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

   const rootProcs = processes.filter((proc) => proc.ppid === 0);
   let tableRows = <EmptyRow/>
   if (rootProcs.length > 0) {
      const allProcs = [rootProcs[0], ...findChildProcs(rootProcs[0], " ")];
      tableRows = allProcs.map((proc) => (
         <tr key={proc.pid} title={proc.command}>
            <td>
               {proc.pidPrefix}
               {proc.pid}
            </td>
            <td>{proc.name}</td>
            <td>{proc.cpu_usage_percent.toFixed(1)}%</td>
            <td>{proc.mem_usage_percent.toFixed(1)}%</td>
         </tr>
      ));
   }

   const tableHeaderRow = (
      <tr>
         <th>PID</th>
         <th>Name</th>
         <th>CPU</th>
         <th>Mem</th>
      </tr>
   );

   return (
      <table className={classes["proc-table"]}>
         <thead>{tableHeaderRow}</thead>
         <tbody>{tableRows}</tbody>
      </table>
   );
};

export default TreeTable;
