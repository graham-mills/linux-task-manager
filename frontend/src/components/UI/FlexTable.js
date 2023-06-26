import classes from "./FlexTable.module.css";

export const Label = (props) => {
   return (
      <div className={classes.label}>
         <b>{props.children}</b>
      </div>
   );
};

export const Value = (props) => {
   return <div className={classes.value}>{props.children}</div>;
};

export const Row = (props) => {
   return <div className={classes.row}>{props.children}</div>;
};

export const Column = (props) => {
   return <div className={classes.column}>{props.children}</div>;
};

export const FlexTable = (props) => {
   const colClass = props.columns === 3 ? classes["table-col-3"] : classes["table-col-2"];

   return <div className={`${classes.table} ${colClass}`}>{props.children}</div>;
};

